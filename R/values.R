### Special support for R-level representations of Qt value types like QColor

"$.RQtValue" <- function(x, name) {
  if (is.list(x) && name %in% names(x))
    x[[name]]
  else function(...) qinvoke(x, name, ...)
}
