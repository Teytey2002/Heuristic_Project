# PFSP Heuristic Optimization Project

A C++ implementation of iterative improvement and Variable Neighborhood Descent (VND) algorithms for the Permutation Flow-Shop Scheduling Problem (PFSP-CT), supported by Python and R scripts for analysis.

---

## 📁 Project Structure
```
Heuristic_Project/
├── src/           # C++ source code
├── data/          # Input instances
├── results/       # Processed result files
├── scripts/       # Python + R analysis scripts
├── report/        # PDF report and slides
├── run_all.sh     # Run all II combinations
├── run_vnd.sh     # Run all VND combinations
```

---

## ⚙️ Compilation
```bash
cd Heuristic_Project
g++ -std=c++17 -O2 -o pfsp src/pfsp.cpp
```

---

## ▶️ Run on one instance
```bash
./pfsp data/instances/Benchmarks/ta051 --first --exchange --random-init --seed=0 --save
```

### To run with VND1 or VND2:
```bash
./pfsp data/instances/Benchmarks/ta051 --vnd1 --srz --seed=0 --save
./pfsp data/instances/Benchmarks/ta051 --vnd2 --srz --seed=0 --save
```
Note: When using `--vnd1` or `--vnd2`, **do not specify a neighborhood**, as the algorithm internally applies a sequence of neighborhoods.

### Available options:
- `--first` / `--best` / `--vnd1` / `--vnd2` : pivoting strategies
- `--exchange` / `--insert` / `--transpose` : neighborhood types (only for II)
- `--random-init` / `--srz` : initial solution method
- `--seed=0` : set random seed
- `--save` : log output to CSV

---

## 🚀 Run all experiments

From project root:
```bash
./run_all.sh   # Run all combinations for II
./run_vnd.sh   # Run VND1 and VND2 on all instances
```

Output is saved in `data/*.csv`

---

## 📊 Analyze results

Inside `scripts/`, use Python to generate metrics:
```bash
python3 analyze_results.py
```
This computes:
- Average deviation from best-known solutions
- Average execution time
- % improvement of VND over single neighborhoods

---

## 📈 Statistical comparison (Wilcoxon)

Use precomputed CSV files of paired results, then run:
```bash
Rscript scripts/stats_tests.R
```
All test outputs are written to:
```
results/result_wilcoxon.txt
```

---

## 🧑‍💻 Author
Project developed by Theo Desoil — ULB, HEUROPT 2025
