#!/bin/bash
set -e

source cluster.env # Include le variabili salvate nel file di configurazione

# Utente che esegue lo script
USER=$(whoami)

# Installa i pacchetti necessari
install_dependencies() {
  echo "[INFO] Aggiorno sistema e installo pacchetti..."
  sudo apt update && sudo apt upgrade -y
  sudo apt install -y nfs-common openmpi-bin openmpi-common libopenmpi-dev
}

# Genera coppia di chiavi SSH senza passphrase
generate_ssh_keys() {
  if [ ! -f .ssh/id_rsa ]; then
    echo "[INFO] Genero chiavi SSH..."
    ssh-keygen -t rsa -b 4096 -f .ssh/id_rsa -N "" -q
  else
    echo "[INFO] Chiavi SSH già esistenti, salto generazione."
  fi
}

# Copia la chiave pubblica del master sugli altri nodi
copy_ssh_key_to_nodes() {
  echo "[INFO] Copio chiave pubblica SSH sugli altri nodi..."
  for node in "${NODES[@]}"; do
    ssh-copy-id -i .ssh/id_rsa.pub $node
  done
}

# Crea il file MPI_HOSTFILE necessario per distribuire il carico con mpirun
create_mpi_hostfile() {
  echo "[INFO] Creo hostfile MPI in $MPI_HOSTFILE ..."
  printf "%s\n" "${MASTER[@]}" > $MPI_HOSTFILE
  printf "%s\n" "${NODES[@]}" >> $MPI_HOSTFILE
  chown $USER:$USER $MPI_HOSTFILE
}

# Configura server NFS (solo master)
setup_nfs_server() {
  echo "[INFO] Configuro server NFS..."
  sudo apt install -y nfs-kernel-server

  # Crea directory da condividere se non esiste
  sudo mkdir -p $NFS_DIR
  sudo chown $USER:$USER $NFS_DIR
  sudo chmod 777 $NFS_DIR  # puoi adattare i permessi

  # Aggiunge l'export NFS se non già presente
  EXPORT_LINE="$(realpath $WORK_DIR/$NFS_DIR) *(rw,sync,no_subtree_check,no_root_squash)"
  if ! grep -qF "$EXPORT_LINE" /etc/exports; then
    echo "$EXPORT_LINE" | sudo tee -a /etc/exports
  fi

  # Riavvia il servizio NFS
  sudo exportfs -ra
  sudo systemctl restart nfs-kernel-server
}

# Configura client NFS (master e nodi)
setup_nfs_client() {
  echo "[INFO] Configuro client NFS..."
  sudo apt install -y nfs-common

  # Crea punto di mount
  sudo mkdir -p $(realpath $WORK_DIR/$NFS_DIR)

  # Verifica se la condivisione è già montata
  if ! mountpoint -q $(realpath $WORK_DIR/$NFS_DIR); then
    sudo mount -t nfs "rpi-cluster-one:$(realpath $WORK_DIR/$NFS_DIR)" "$(realpath $WORK_DIR/$NFS_DIR)" #monto il filesystem
    
    #Persistenza del file system dopo i reboot inserendo una entry in /etc/fstab
    FSTAB_ENTRY="rpi-cluster-one:$(realpath $WORK_DIR/$NFS_DIR)   $(realpath $WORK_DIR/$NFS_DIR)   nfs   rw,_netdev,noatime,x-systemd.automount,x-systemd.idle-timeout=60   0  0"

    # Controlla se la riga esiste già
    if ! grep -q "rpi-cluster-one:$(realpath $WORK_DIR/$NFS_DIR)" /etc/fstab; then
        echo "Aggiungo mount persistente in /etc/fstab..."
        echo "$FSTAB_ENTRY" | sudo tee -a /etc/fstab
    else
        echo "Entry già presente in /etc/fstab, nessuna modifica."
    fi

    # Ricarica fstab
    sudo systemctl daemon-reload
    sudo systemctl restart remote-fs.target || sudo mount -a
    
  else
    echo "[INFO] NFS già montato in $NFS_DIR"
  fi
}

# Esecuzione in base al ruolo
ROLE=$1

if [ "$ROLE" == "master" ]; then
  install_dependencies
  generate_ssh_keys
  copy_ssh_key_to_nodes
  create_mpi_hostfile
  setup_nfs_server
  echo "[INFO] Setup master completato."

elif [ "$ROLE" == "node" ]; then
  install_dependencies
  create_mpi_hostfile
  setup_nfs_client
  echo "[INFO] Setup nodo completato."

else
  echo "Uso: $0 {master|node}"
  exit 1
fi

