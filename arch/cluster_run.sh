#!/bin/bash
set -e

source cluster.env # Include le variabili salvate nel file di configurazione

# Verifica che sono presenti tutti gli argomenti necessari
if [ $# -lt 4 ]; then
  echo "Usage: $0 <binary_name> <folder> [--all-cores | --one-core ] -- [args for <binary_name>]"
  exit 1
fi

# Salva nelle variabili gli argomenti
BINARY_NAME="$1" 
shift 

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

# Cartella dove copiare $FOLDER
REMOTE_NFS_PATH="$WORK_DIR/$NFS_DIR"

# Percorso della cartella remota contenente gli script
REMOTE_PROGRAM_FOLDER="$REMOTE_NFS_PATH/$(basename "$FOLDER")"

# Copia la cartella $FOLDER (contenente gli script) nella cartella condivisa sul master (e quindi accesibile anche dagli altri nodi)
copy_script() {
  echo "[INFO] Copying $FOLDER to $MASTER..."
  scp -r "$FOLDER" "$MASTER:$REMOTE_NFS_PATH/"  
}

# Compila gli script attraverso il Makefile nella cartella
compile_script() {
  echo "[INFO] Compiling with make on $MASTER..."
  ssh "$MASTER" "cd $REMOTE_PROGRAM_FOLDER && make"
}

# Esegue il programma
run_script() {
  echo "[INFO] Running binary on cluster using $MPI_HOSTFILE..."

  if [ "$CORE_MODE" = "ALL" ]; then
    ssh "$MASTER" "mpirun -tag-output -np $(($NUM_NODES * $NUM_CORES_PER_NODE)) --hostfile $WORK_DIR/$MPI_HOSTFILE $REMOTE_PROGRAM_FOLDER/$BINARY_NAME ${PROGRAM_ARGS[@]} " 
  
  elif [ "$CORE_MODE" = "ONE" ]; then
    ssh "$MASTER" "mpirun -tag-output -np $NUM_NODES --map-by ppr:1:node --hostfile $WORK_DIR/$MPI_HOSTFILE $REMOTE_PROGRAM_FOLDER/$BINARY_NAME ${PROGRAM_ARGS[@]} "
  fi
}

copy_script
compile_script
run_script
