#!/bin/bash

EXEC=./pfsp
INSTANCES_DIR=./instances/Benchmarks
RESULTS=results.csv
JOBS_AT_ONCE=4   # Nombre de jobs en parallèle
counter=0        # Compteur de jobs

PIVOTS=("first" "best")
NEIGHBORS=("transpose" "exchange" "insert")
INITS=("random-init" "srz")
SEEDS=(1 2 3 4 5 6 7 8 9 10)

# Nettoyage du fichier de sortie
echo "instance,jobs,pivot,neighborhood,init,seed,completion,time" > $RESULTS

# Boucle sur toutes les combinaisons
for instance in $INSTANCES_DIR/*; do
  for pivot in "${PIVOTS[@]}"; do
    for neighbor in "${NEIGHBORS[@]}"; do
      for init in "${INITS[@]}"; do
        if [[ "$init" == "random-init" ]]; then
          for seed in "${SEEDS[@]}"; do
            echo ">>> $instance | $pivot | $neighbor | $init | seed $seed"
            $EXEC "$instance" --$pivot --$neighbor --$init --seed=$seed --save &

            ((counter++))
            if (( counter % JOBS_AT_ONCE == 0 )); then
              wait  # ⏳ Attend que les N jobs en cours se terminent
            fi
          done
        else
          echo ">>> $instance | $pivot | $neighbor | $init"
          $EXEC "$instance" --$pivot --$neighbor --$init --save &
          
          ((counter++))
          if (( counter % JOBS_AT_ONCE == 0 )); then
            wait
          fi
        fi
      done
    done
  done
done

# On attend les derniers jobs restants
wait
