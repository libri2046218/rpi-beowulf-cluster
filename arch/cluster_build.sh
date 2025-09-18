#!/bin/bash
set -e

source cluster.env # Include le variabili salvate nel file di configurazione

SCRIPT_DIR=$(pwd)             # directory locale contenente setup.sh e rollback.sh
SETUP_FILE="setup.sh"         # script di setup
ROLLBACK_FILE="rollback.sh"   # script di rollback
ENV_FILE="cluster.env"        # variabili di ambiente

# Copia i file sul nodo
copy_scripts() {
  local node=$1
  echo "[INFO] Copio script su $node..."
  scp "$SCRIPT_DIR/$SETUP_FILE" "$SCRIPT_DIR/$ROLLBACK_FILE" "$SCRIPT_DIR/$ENV_FILE" "$node:$WORK_DIR"
}

# Cancella i file dal nodo
remove_scripts() {
  local node=$1
  echo "[INFO] Rimuovo script su $node..."
  ssh "$node" "rm -f $WORK_DIR/$SETUP_FILE $WORK_DIR/$ROLLBACK_FILE $WORK_DIR/$ENV_FILE" 
}

# Esegue lo script sul nodo
run_script() {
  local node=$1
  local role=$2
  local action=$3
  echo "[INFO] Eseguo $action su $node..."

  if [ "$action" = "setup" ]; then
    ssh -t $node "bash $WORK_DIR/$SETUP_FILE $role"
  elif [ "$action" = "rollback" ]; then
    ssh -t $node "bash $WORK_DIR/$ROLLBACK_FILE $role"
  else
    echo "[ERROR] Azione non eseguibile"
  fi 
}

case "$1" in
  setup)
    echo "[INFO] Avvio setup del cluster..."

    # Copia ed esegue setup sul master
    copy_scripts "$MASTER"
    run_script "$MASTER" master setup

    # In parallelo copia file di setup sui nodi
    for node in "${NODES[@]}"; do
      copy_scripts "$node" &
    done
    wait

    # In parallelo esegue setup sui nodi
    for node in "${NODES[@]}"; do
      run_script "$node" node setup &
    done
    wait

    echo "[INFO] Setup completato su tutti i nodi."
    ;;

  rollback)
    echo "[INFO] Avvio rollback del cluster..."

    # In parallelo copia file di rollback sui nodi
    for node in "${NODES[@]}"; do
      copy_scripts "$node" &
    done
    wait

    # In parallelo esegue rollback sui nodi
    for node in "${NODES[@]}"; do
      run_script "$node" node rollback &
    done
    wait

    # In parallelo cancella file sui nodi
    for node in "${NODES[@]}"; do
      remove_scripts "$node" &
    done
    wait

    # Copia ed esegue rollback sul master
    copy_scripts "$MASTER"
    run_script "$MASTER" master rollback
    remove_scripts "$MASTER"

    echo "[INFO] Rollback completato su tutti i nodi."
    ;;

  *)
    echo "Uso: $0 {setup|rollback}"
    exit 1
    ;;
esac
