import pandas as pd
from pathlib import Path

df = pd.read_csv("results.csv")

neighs = ["insert", "exchange", "transpose"]
inits = ["random", "srz"]

output_dir = Path("wilcoxon_q2")
output_dir.mkdir(exist_ok=True)

for neigh in neighs:
    for init in inits:
        df_first = df[
            (df["pivot"] == "first") &
            (df["neighborhood"] == neigh) &
            (df["init"] == init)
        ]
        df_best = df[
            (df["pivot"] == "best") &
            (df["neighborhood"] == neigh) &
            (df["init"] == init)
        ]

        merged = df_first.merge(df_best, on="instance", suffixes=("_first", "_best"))
        result = merged[["completion_first", "completion_best"]]
        result.columns = ["first", "best"]

        filename = f"wilcoxon_q2_{neigh}_{init}.csv"
        result.to_csv(output_dir / filename, index=False)

print("✅ Tous les fichiers Q2 générés dans le dossier 'wilcoxon_q2/' !")
