qmethods <- function(x) {
  stopifnot(is(x, "QObject"))
  methods <- .Call(qt_qmethods, x)
  methods[[1]] <- c("method", "signal", "slot", "constructor")[methods[[1]] + 1]
  names(methods) <- c("type", "signature", "return")
  as.data.frame(methods)
}

qnormalizedSignature <- function(x) {
  .Call(qt_qnormalizedSignature, as.character(x))
}
