## simple script for converting C++ Qt code to R Qt code

qtranslate <- function(f) {
  lines <- readLines(f)
  table <- c("new " = "Qt$", "->" = "$", ".* \\*" = "", ";" = "", "=" = "<-")
  for(i in seq_along(table))
    lines <- gsub(names(table)[i], table[i], lines)
  rf <- sub("cpp$", "R", f)
  writeLines(lines, rf)
}
