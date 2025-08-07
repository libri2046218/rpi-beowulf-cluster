#!/bin/bash
set -e

# Lista nodi (modifica se serve)
USER=$(whoami)
HOSTS_FILE="cluster_hosts.txt"

readarray -t MASTER < <(awk '/\[master\]/{f=1; next} /\[/{f=0} f' "$HOSTS_FILE")
readarray -t NODES < <(awk '/\[nodes\]/{f=1; next} /\[/{f=0} f' "$HOSTS_FILE")

MPI_HOSTFILE="mpi_hostfile"
NFS_DIR="nfs_shared"
NFS_MOUNT="nfs_shared"
WORK_DIR=$2

# Funzione per aggiornare e installare pacchetti
install_dependencies() {
  echo "[INFO] Aggiorno sistema e installo pacchetti..."
  sudo apt update && sudo apt upgrade -y
  sudo apt install -y nfs-common openmpi-bin openmpi-common libopenmpi-dev
}

# Funzione per generare coppia di chiavi SSH senza passphrase
generate_ssh_keys() {
  if [ ! -f .ssh/id_rsa ]; then
    echo "[INFO] Genero chiavi SSH..."
    ssh-keygen -t rsa -b 4096 -f .ssh/id_rsa -N "" -q
  else
    echo "[INFO] Chiavi SSH già esistenti, salto generazione."
  fi
}

# Funzione per copiare chiave pubblica del master sugli altri nodi
copy_ssh_key_to_nodes() {
  echo "[INFO] Copio chiave pubblica SSH sugli altri nodi..."
  for node in "${NODES[@]}"; do
    ssh-copy-id -i .ssh/id_rsa.pub $node
  done
}

# Funzione per creare file hostfile MPI
create_mpi_hostfile() {
  echo "[INFO] Creo hostfile MPI in $MPI_HOSTFILE ..."
  printf "%s\n" "${MASTER[@]}" > $MPI_HOSTFILE
  printf "%s\n" "${NODES[@]}" > $MPI_HOSTFILE
  chown $USER:$USER $MPI_HOSTFILE
}

# Configurazione server NFS (solo master)
setup_nfs_server() {
  echo "[INFO] Configuro server NFS..."
  sudo apt install -y nfs-kernel-server

  # Creo directory da condividere se non esiste
  sudo mkdir -p $NFS_DIR
  sudo chown $USER:$USER $NFS_DIR
  sudo chmod 777 $NFS_DIR  # puoi adattare i permessi

  # Aggiungo l'export NFS se non già presente
  EXPORT_LINE="$(realpath $WORK_DIR/$NFS_DIR) *(rw,sync,no_subtree_check,no_root_squash)"
  if ! grep -qF "$EXPORT_LINE" /etc/exports; then
    echo "$EXPORT_LINE" | sudo tee -a /etc/exports
  fi

  # Riavvio il servizio NFS
  sudo exportfs -ra
  sudo systemctl restart nfs-kernel-server
}

# Configurazione client NFS (master e nodi)
setup_nfs_client() {
  echo "[INFO] Configuro client NFS..."
  sudo apt install -y nfs-common

  # Creo punto di mount
  sudo mkdir -p $(realpath $WORK_DIR/$NFS_MOUNT)

  # Verifico se la condivisione è già montata
  if ! mountpoint -q $(realpath $WORK_DIR/$NFS_MOUNT); then
    sudo mount -t nfs "rpi-cluster-one:$(realpath $WORK_DIR/$NFS_DIR)" "$(realpath $WORK_DIR/$NFS_MOUNT)" #monto il filesystem
  else
    echo "[INFO] NFS già montato in $NFS_MOUNT"
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

