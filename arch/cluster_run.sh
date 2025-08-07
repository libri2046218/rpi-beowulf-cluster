#!/bin/bash
set -e

# === USAGE CHECK ===
if [ $# -ne 4 ]; then
  echo "Usage: $0 <host_file> <source_file.c> <binary_name> <core_num>"
  echo "Example: $0 cluster/cluster_hosts.txt src/hello.c hello 12"
  exit 1
fi

# === INPUT PARAMETERS ===
HOSTS_FILE="$1"         
SCRIPT_FILE="$2"        
SCRIPT_NAME="$3"        
CORE_NUM=$4


# === CONFIGURATION ===
NFS_DIR="nfs_shared"    # Folder shared via NFS, under remote ~
WORK_DIR="~"            # Remote working directory
MPI_HOSTFILE="mpi_hostfile"  # Hostfile path under NFS_DIR

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
  ssh "$MASTER" "mpirun -np $CORE_NUM --hostfile $WORK_DIR/$MPI_HOSTFILE $REMOTE_NFS_PATH/$SCRIPT_NAME"
}

# === RUN ===
copy_script
compile_script
run_script
