import pandas as pd

# Lire les résultats expérimentaux
df = pd.read_csv("results.csv")

# Extraire le nom court sans chemin ni extension
df["instance_short"] = df["instance"].apply(lambda x: x.split("/")[-1].split(".")[0].lower())

# Lire les meilleurs résultats depuis le fichier txt (avec séparateur virgule)
best = pd.read_csv("bestKnownTCT.txt", sep=",", skipinitialspace=True)

# Nettoyer les noms pour correspondre à ceux des instances
best["instance"] = best["INS"].str.lower().apply(lambda x: "ta" + x[2:].zfill(3))
best = best.rename(columns={"VAL": "best_known"})

# Fusion
df = df.merge(best[["instance", "best_known"]], left_on="instance_short", right_on="instance")

# Calcul de la déviation
df["deviation"] = 100 * (df["completion"] - df["best_known"]) / df["best_known"]

# Ajouter colonne taille du problème
df["size"] = df["jobs"]

# Clé d’algorithme : pivot + voisinage
df["algo"] = df["pivot"] + "-" + df["neighborhood"]

# Groupement et résumé
summary = df.groupby(["algo", "init", "size"]).agg({
    "deviation": "mean",
    "time": "sum"
}).reset_index()

# Renommer colonnes pour export
summary = summary.rename(columns={
    "deviation": "avg_deviation(%)",
    "time": "total_time(s)"
})

# Sauvegarde
summary.to_csv("summary_results.csv", index=False)
print(summary)
