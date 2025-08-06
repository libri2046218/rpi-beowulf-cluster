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

# Rimuove i pacchetti installati (MPI e NFS)
remove_dependencies() {
  echo "[INFO] Rimuovo pacchetti installati..."
  sudo apt purge -y openmpi-bin openmpi-common libopenmpi-dev
  sudo apt autoremove -y
}

# Rimuove chiavi SSH solo se generate
remove_ssh_keys() {
  echo "[INFO] Rimuovo chiavi SSH..."
  if [ -f .ssh/id_rsa ]; then
    rm -f .ssh/id_rsa .ssh/id_rsa.pub
  fi
}

# Rimuove chiave pubblica SSH da nodi
escape_for_sed() {
  echo "$1" | sed -e 's/[\/&]/\\&/g'
}

remove_ssh_keys_from_nodes() {
  echo "[INFO] Rimuovo chiave pubblica SSH dai nodi..."
  PUB_KEY=$(cat .ssh/id_rsa.pub 2>/dev/null || echo "")
  if [ -z "$PUB_KEY" ]; then
    echo "[WARN] Chiave pubblica SSH non trovata, salto rimozione."
    return
  fi

  ESCAPED_KEY=$(escape_for_sed "$PUB_KEY")

  for node in "${NODES[@]}"; do
    ssh $node "sed -i '/$ESCAPED_KEY/d' ~/.ssh/authorized_keys || true"
  done
}


# Rimuove il file hostfile MPI
remove_mpi_hostfile() {
  echo "[INFO] Rimuovo file hostfile MPI..."
  [ -f $HOSTFILE ] && rm -f $HOSTFILE
}

# Rimuove configurazione server NFS (solo master)
remove_nfs_server() {
  echo "[INFO] Rimuovo configurazione server NFS..."

  # Rimuove export da /etc/exports
  EXPORT_LINE="$NFS_DIR *(rw,sync,no_subtree_check,no_root_squash)"
  sudo sed -i "\|$EXPORT_LINE|d" /etc/exports

  sudo exportfs -ra
  sudo systemctl restart nfs-kernel-server

  # Rimuove directory condivisa
  sudo umount -f "$NFS_DIR" 2>/dev/null || true
  sudo rm -rf "$NFS_DIR"

  # Rimuove pacchetto
  sudo apt purge -y nfs-kernel-server
}

# Rimuove configurazione client NFS (master e nodi)
remove_nfs_client() {
  echo "[INFO] Rimuovo configurazione client NFS..."

  # Smonta se montato
  if mountpoint -q $NFS_MOUNT; then
    sudo umount $NFS_MOUNT
  fi

  sudo rm -rf $NFS_MOUNT
  sudo apt purge -y nfs-common
}

# Esecuzione in base al ruolo
ROLE=$1

if [ "$ROLE" == "master" ]; then
  remove_nfs_server
  remove_mpi_hostfile
  remove_ssh_keys_from_nodes
  remove_ssh_keys
  remove_dependencies
  echo "[INFO] Rollback master completato."

elif [ "$ROLE" == "node" ]; then
  remove_nfs_client
  remove_mpi_hostfile
  remove_dependencies
  echo "[INFO] Rollback nodo completato."

else
  echo "Uso: $0 {master|node}"
  exit 1
fi

