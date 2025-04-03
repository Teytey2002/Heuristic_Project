# === 1. Charger les données ===
df <- read.csv("vnd_results.csv")

# === 2. Extraire le nom court de l'instance ===
df$instance_short <- toupper(gsub(".*\\/([^\\.]+)\\.txt", "\\1", df$instance))

# === 3. Filtrer uniquement SRZ + VND1/VND2 ===
df_vnd <- subset(df, init == "srz" & pivot %in% c("VND1", "VND2"))

# === 4. Créer un tableau large avec VND1 vs VND2 côte à côte ===
library(tidyr)
library(dplyr)

wide_df <- df_vnd %>%
  select(instance_short, pivot, completion) %>%
  pivot_wider(names_from = pivot, values_from = completion) %>%
  drop_na()

# === 5. Test de Wilcoxon (paired) ===
test_result <- wilcox.test(wide_df$VND1, wide_df$VND2, paired = TRUE)

# === 6. Afficher les résultats ===
print("=== Wilcoxon signed-rank test between VND1 and VND2 ===")
print(test_result)

# === 7. Optionnel : Sauvegarder les valeurs dans un fichier CSV ===
comparison <- data.frame(
  instance = wide_df$instance_short,
  VND1 = wide_df$VND1,
  VND2 = wide_df$VND2,
  difference = wide_df$VND1 - wide_df$VND2
)
write.csv(comparison, "vnd1_vs_vnd2_values.csv", row.names = FALSE)
