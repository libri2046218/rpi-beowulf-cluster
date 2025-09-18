#!/bin/bash
set -e

# Legge nodi master e worker da un file esterno
HOSTS_FILE="cluster_hosts.txt"
readarray -t MASTER < <(awk '/\[master\]/{f=1; next} /\[/{f=0} f' "$HOSTS_FILE")
readarray -t NODES < <(awk '/\[nodes\]/{f=1; next} /\[/{f=0} f' "$HOSTS_FILE")

SCRIPT_DIR=$(pwd)  # directory locale contenente setup.sh e rollback.sh
SETUP_FILE="setup.sh" #script di setup
ROLLBACK_FILE="rollback.sh" #script di rollback
WORK_DIR="~" #cartella di lavoro su ogni nodo del cluster

copy_scripts() {
  local node=$1
  echo "[INFO] Copio script su $node..."
  scp "$SCRIPT_DIR/$SETUP_FILE" "$SCRIPT_DIR/$ROLLBACK_FILE" "$SCRIPT_DIR/$HOSTS_FILE" "$node:$WORK_DIR"
}

remove_scripts() {
  local node=$1
  echo "[INFO] Rimuovo script su $node..."
  ssh "$node" "rm -f $WORK_DIR/$SETUP_FILE $WORK_DIR/$ROLLBACK_FILE $WORK_DIR/$HOSTS_FILE" 
}

run_script() {
  local node=$1
  local role=$2
  local action=$3
  echo "[INFO] Eseguo $action su $node..."

  if [ "$action" = "setup" ]; then
    ssh -t $node "bash $WORK_DIR/$SETUP_FILE $role $WORK_DIR"
  elif [ "$action" = "rollback" ]; then
    ssh -t $node "bash $WORK_DIR/$ROLLBACK_FILE $role $WORK_DIR"
  else
    echo "[ERROR] Azione non eseguibile"
  fi 
}

case "$1" in
  setup)
    echo "[INFO] Avvio setup del cluster..."

    # Step 1: Copia ed esegue setup sul master
    copy_scripts "$MASTER"
    run_script "$MASTER" master setup

    # Step 2: In parallelo copia ed esegue setup sui nodi
    for node in "${NODES[@]}"; do
      copy_scripts "$node" &
    done
    wait
    for node in "${NODES[@]}"; do
      run_script "$node" node setup &
    done
    wait

    echo "[INFO] Setup completato su tutti i nodi."
    ;;

  rollback)
    echo "[INFO] Avvio rollback del cluster..."

    # Step 1: In parallelo copia ed esegue rollback sui nodi
    for node in "${NODES[@]}"; do
      copy_scripts "$node" &
    done
    wait
    for node in "${NODES[@]}"; do
      run_script "$node" node rollback &
    done
    wait

    for node in "${NODES[@]}"; do
      remove_scripts "$node" &
    done
    wait

    # Step 2: Copia ed esegue rollback sul master
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
