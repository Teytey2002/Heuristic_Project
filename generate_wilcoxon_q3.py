import pandas as pd
from pathlib import Path

df = pd.read_csv("results.csv")

pivots = ["first", "best"]
inits = ["random", "srz"]
neigh_pairs = [("insert", "exchange"), ("insert", "transpose"), ("exchange", "transpose")]

output_dir = Path("wilcoxon_q3")
output_dir.mkdir(exist_ok=True)

for pivot in pivots:
    for init in inits:
        for n1, n2 in neigh_pairs:
            df1 = df[
                (df["pivot"] == pivot) &
                (df["neighborhood"] == n1) &
                (df["init"] == init)
            ].copy()
            df2 = df[
                (df["pivot"] == pivot) &
                (df["neighborhood"] == n2) &
                (df["init"] == init)
            ].copy()

            # Important : filtrer sur même instance ET même seed
            merged = df1.merge(df2, on=["instance", "seed"], suffixes=(f"_{n1}", f"_{n2}"))

            result = merged[[f"completion_{n1}", f"completion_{n2}"]]
            result.columns = [n1, n2]

            filename = f"wilcoxon_q3_{pivot}_{init}_{n1}_vs_{n2}.csv"
            result.to_csv(output_dir / filename, index=False)

print("✅ Tous les fichiers Q3 régénérés proprement dans 'wilcoxon_q3/'")
