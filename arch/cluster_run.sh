#!/bin/bash
set -e

# === USAGE CHECK ===
if [ $# -lt 4 ]; then
  echo "Usage: $0 <host_file> <binary_name> <folder> [--all-cores | --one-core ] -- [args for <binary_name>]"
  exit 1
fi

# === INPUT PARAMETERS ===
HOSTS_FILE="$1"         
SCRIPT_NAME="$2" 
shift 2

FOLDER="$1"
shift

OPTION="$1"
shift

case "$OPTION" in
    --all-cores)
        CORE_MODE="ALL"
        ;;
    --one-core)
        CORE_MODE="ONE"
        ;;
    *)
        echo "Errore: opzione non valida $4"
        exit 1
        ;;
esac

PROGRAM_ARGS=()

if [[ "$1" == "--" ]]; then
  shift
  for arg in "$@"; do
    PROGRAM_ARGS+=("$arg")
  done
fi

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

REMOTE_PROGRAM_FOLDER="$REMOTE_NFS_PATH/$(basename "$FOLDER")"

# === FUNCTIONS ===

copy_script() {
  echo "[INFO] Copying $FOLDER to $MASTER..."
  scp -r "$FOLDER" "$MASTER:$REMOTE_NFS_PATH/"  
}

compile_script() {
  echo "[INFO] Compiling with make on $MASTER..."
  ssh "$MASTER" "cd $REMOTE_PROGRAM_FOLDER && make"
}

run_script() {
  echo "[INFO] Running binary on cluster using $MPI_HOSTFILE..."

  if [ "$CORE_MODE" = "ALL" ]; then
    ssh "$MASTER" "mpirun -tag-output -np $(($NUM_NODES * $NUM_CORES_PER_NODE)) --hostfile $WORK_DIR/$MPI_HOSTFILE $REMOTE_PROGRAM_FOLDER/$SCRIPT_NAME ${PROGRAM_ARGS[@]} " 
  
  elif [ "$CORE_MODE" = "ONE" ]; then
    ssh "$MASTER" "mpirun -tag-output -np $NUM_NODES --map-by ppr:1:node --hostfile $WORK_DIR/$MPI_HOSTFILE $REMOTE_PROGRAM_FOLDER/$SCRIPT_NAME ${PROGRAM_ARGS[@]} "
  fi
}

# === RUN ===
copy_script
compile_script
run_script
