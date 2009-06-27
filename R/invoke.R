qinvoke <- function(x, method, ...) {
  stopifnot(is(x, "QObject"))
  method <- qresolveSignature(x, as.character(method))
  args <- list(...)
  invisible(.Call(qt_qinvoke, x, method, args))
}
