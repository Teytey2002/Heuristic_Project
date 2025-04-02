#!/bin/bash

EXEC=./pfsp
INSTANCES_DIR=./instances/Benchmarks
RESULTS=results.csv

PIVOTS=("first" "best")
NEIGHBORS=("transpose" "exchange" "insert")
INITS=("random-init" "srz")
SEEDS=(1 2 3 4 5 6 7 8 9 10)

# Réinitialiser le fichier de résultats avec l’en-tête
echo "instance,jobs,pivot,neighborhood,init,seed,completion,time" > $RESULTS

# Lancer tous les tests
for instance in $INSTANCES_DIR/*; do
  for pivot in "${PIVOTS[@]}"; do
    for neighbor in "${NEIGHBORS[@]}"; do
      for init in "${INITS[@]}"; do
        if [[ "$init" == "random-init" ]]; then
          for seed in "${SEEDS[@]}"; do
            echo ">>> $instance | $pivot | $neighbor | $init | seed $seed"
            $EXEC "$instance" --$pivot --$neighbor --$init --seed=$seed --save
          done
        else
          echo ">>> $instance | $pivot | $neighbor | $init"
          $EXEC "$instance" --$pivot --$neighbor --$init --save
        fi
      done
    done
  done
done
