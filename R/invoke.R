qinvoke <- function(x, method, ...) {
  stopifnot(is(x, "QObject"))
  args <- list(...)
  method <- qresolveSignature(x, as.character(method), nargs=length(args))
  invisible(.Call(qt_qinvoke, x, method, args))
}
