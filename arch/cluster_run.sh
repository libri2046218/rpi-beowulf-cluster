#!/bin/bash
set -e

# === USAGE CHECK ===
if [ $# -ne 4 ]; then
  echo "Usage: $0 <host_file> <source_file.c> <binary_name> [--all-cores | --one-core | --num-core <n>]"
  exit 1
fi

# === INPUT PARAMETERS ===
HOSTS_FILE="$1"         
SCRIPT_FILE="$2"        
SCRIPT_NAME="$3"        


case "$4" in
    --all-cores)
        CORE_MODE="ALL"
        ;;
    --one-core)
        CORE_MODE="ONE"
        ;;
    --num-core)
        if [ -z "$5" ]; then
            echo "Errore: devi specificare un numero dopo --num-core"
            exit 1
        fi
        CORE_MODE="$5"
        shift
        ;;
    *)
        echo "Errore: opzione non valida $4"
        exit 1
        ;;
esac

# === CONFIGURATION ===
NFS_DIR="nfs_shared"    # Folder shared via NFS, under remote ~
WORK_DIR="~"            # Remote working directory
MPI_HOSTFILE="mpi_hostfile"  # Hostfile path under NFS_DIR
NUM_NODES=3             #Number of nodes 
NUM_CORES_PER_NODE=4    #Number of core per nodes

# === PARSE MASTER AND NODES ===
readarray -t MASTER < <(awk '/\[master\]/{f=1; next} /\[/{f=0} f' "$HOSTS_FILE")
readarray -t NODES < <(awk '/\[nodes\]/{f=1; next} /\[/{f=0} f' "$HOSTS_FILE")

REMOTE_NFS_PATH="$WORK_DIR/$NFS_DIR"
SCRIPT_BASENAME=$(basename "$SCRIPT_FILE")  # just filename from path

# === FUNCTIONS ===

copy_script() {
  echo "[INFO] Copying $SCRIPT_FILE to $MASTER..."
  scp "$SCRIPT_FILE" "$MASTER:$REMOTE_NFS_PATH/$SCRIPT_BASENAME"
}

compile_script() {
  echo "[INFO] Compiling with mpicc on $MASTER..."
  ssh "$MASTER" "mpicc -o $REMOTE_NFS_PATH/$SCRIPT_NAME $REMOTE_NFS_PATH/$SCRIPT_BASENAME"
}

run_script() {
  echo "[INFO] Running binary on cluster using $MPI_HOSTFILE..."

  if [ "$CORE_MODE" = "ALL" ]; then
    ssh "$MASTER" "mpirun -tag-output -np $(($NUM_NODES * $NUM_CORES_PER_NODE)) --hostfile $WORK_DIR/$MPI_HOSTFILE $REMOTE_NFS_PATH/$SCRIPT_NAME"
  
  elif [ "$CORE_MODE" = "ONE" ]; then
    ssh "$MASTER" "mpirun -tag-output -np $NUM_NODES --map-by ppr:1:node --hostfile $WORK_DIR/$MPI_HOSTFILE $REMOTE_NFS_PATH/$SCRIPT_NAME"
  
  else
      echo "==> Lancio su $CORE_MODE core" #TODO
      echo "Non ancora implementato"
  fi
  
  
}

# === RUN ===
copy_script
compile_script
run_script
