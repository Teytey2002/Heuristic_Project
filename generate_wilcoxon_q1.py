import pandas as pd
from pathlib import Path

# Charger les données
df = pd.read_csv("results.csv")

# Pivots et neighborhoods à comparer
pivots = ["first", "best"]
neighs = ["insert", "exchange", "transpose"]

output_dir = Path("wilcoxon_q1")
output_dir.mkdir(exist_ok=True)

for pivot in pivots:
    for neigh in neighs:
        df_r = df[
            (df["pivot"] == pivot) &
            (df["neighborhood"] == neigh) &
            (df["init"] == "random")
        ]
        df_srz = df[
            (df["pivot"] == pivot) &
            (df["neighborhood"] == neigh) &
            (df["init"] == "srz")
        ]

        merged = df_r.merge(df_srz, on="instance", suffixes=("_random", "_srz"))
        result = merged[["completion_random", "completion_srz"]]
        result.columns = ["random", "srz"]

        filename = f"wilcoxon_q1_{pivot}_{neigh}.csv"
        result.to_csv(output_dir / filename, index=False)

print("Tous les fichiers Q1 générés dans le dossier 'wilcoxon_q1/' !")
