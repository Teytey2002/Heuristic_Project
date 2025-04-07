#setwd("wilcoxon_q3")

comparaisons <- list.files(pattern = "^wilcoxon_q3_.*\\.csv$")

for (file in comparaisons) {
  cat("\n====", file, "====\n")
  data <- read.csv(file)

  # Vérifie qu’il y a bien 2 colonnes
  if (ncol(data) != 2) {
    cat("⚠️  Fichier invalide (pas deux colonnes)\n")
    next
  }

  colnames <- colnames(data)
  x <- as.numeric(data[[1]])
  y <- as.numeric(data[[2]])

  # Vérifie qu’il y a assez de données valides
  keep <- complete.cases(x, y)
  if (sum(keep) < 5) {
    cat("⚠️  Pas assez de données pour un test Wilcoxon\n")
    next
  }

  test_result <- wilcox.test(x[keep], y[keep], paired=TRUE)
  print(test_result)
}
