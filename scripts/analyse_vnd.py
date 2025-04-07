import pandas as pd
import os

# === 1. Chargement des fichiers ===
vnd_file = "vnd_results.csv"
ii_file = "results.csv"
best_file = "bestKnownTCT.txt"

vnd_df = pd.read_csv(vnd_file)
ii_df = pd.read_csv(ii_file)
best_df = pd.read_csv(best_file, sep=",", skipinitialspace=True)

# === 2. Nettoyage des noms d’instances ===
def format_instance(x):
    return os.path.basename(x).replace(".txt", "").replace("ta0", "TA").replace("ta", "TA").upper()

vnd_df["instance_short"] = vnd_df["instance"].apply(format_instance)
ii_df["instance_short"] = ii_df["instance"].apply(format_instance)
best_df["INS"] = best_df["INS"].str.strip().str.upper()

# === 3. Fusion avec les meilleures valeurs ===
vnd_df = vnd_df.merge(best_df, left_on="instance_short", right_on="INS", how="left")
ii_df = ii_df.merge(best_df, left_on="instance_short", right_on="INS", how="left")

# === 4. Calcul de la déviation ===
vnd_df["deviation_%"] = 100 * (vnd_df["completion"] - vnd_df["VAL"]) / vnd_df["VAL"]
ii_df["deviation_%"] = 100 * (ii_df["completion"] - ii_df["VAL"]) / ii_df["VAL"]

# === 5. Résumé VND (déviation et temps par pivot et taille) ===
summary_vnd = vnd_df.groupby(["pivot", "jobs"]).agg(
    avg_deviation_percent=("deviation_%", "mean"),
    avg_time=("time", "mean")
).reset_index()

# === 6. Résumé algos simples pour exchange et insert ===
ii_filtered = ii_df[
    (ii_df["pivot"] == "first") & 
    (ii_df["init"] == "srz") &
    (ii_df["neighborhood"].isin(["exchange", "insert"]))
]

summary_ii = ii_filtered.groupby(["neighborhood", "jobs"]).agg(
    avg_deviation_percent=("deviation_%", "mean")
).reset_index()

# === 7. Comparaison : amélioration de VND sur exchange et insert ===
def calc_improvement(vnd_type, simple_type):
    merged = summary_vnd[summary_vnd["pivot"] == vnd_type].merge(
        summary_ii[summary_ii["neighborhood"] == simple_type],
        on="jobs",
        suffixes=("_vnd", "_simple")
    )
    merged["improvement_%"] = 100 * (
        (merged["avg_deviation_percent_simple"] - merged["avg_deviation_percent_vnd"]) /
        merged["avg_deviation_percent_simple"]
    )
    merged["compared_to"] = simple_type
    merged["vnd"] = vnd_type
    return merged[["jobs", "vnd", "compared_to", "improvement_%"]]

improv_vnd1_exchange = calc_improvement("VND1", "exchange")
improv_vnd1_insert = calc_improvement("VND1", "insert")
improv_vnd2_exchange = calc_improvement("VND2", "exchange")
improv_vnd2_insert = calc_improvement("VND2", "insert")

improvement_df = pd.concat([
    improv_vnd1_exchange, improv_vnd1_insert,
    improv_vnd2_exchange, improv_vnd2_insert
]).reset_index(drop=True)

# === 8. Export des résultats ===
vnd_df.to_csv("vnd_full_results_with_deviation.csv", index=False)
summary_vnd.to_csv("vnd_summary_by_algo.csv", index=False)
improvement_df.to_csv("vnd_improvement_over_single_neighborhood.csv", index=False)

print("✅ Analyse complète terminée. 3 fichiers générés :")
print("- vnd_full_results_with_deviation.csv")
print("- vnd_summary_by_algo.csv")
print("- vnd_improvement_over_single_neighborhood.csv")
