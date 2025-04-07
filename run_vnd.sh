#!/bin/bash

EXEC=./pfsp
INSTANCES_DIR=./data/instances/Benchmarks
RESULTS=./results/vnd_results.csv

VNDS=("vnd1" "vnd2")  # VND1 = transpose→exchange→insert, VND2 = transpose→insert→exchange
INIT="srz"

# Réinitialiser le fichier de résultats avec l’en-tête
echo "instance,jobs,pivot,neighborhood,init,seed,completion,time" > $RESULTS

# Exécuter VND1 et VND2 sur chaque instance
for instance in $INSTANCES_DIR/*; do
  for vnd in "${VNDS[@]}"; do
    echo ">>> $instance | $vnd | $INIT"
    $EXEC "$instance" --$vnd --$INIT --save
  done
done
