
## TODO: possibly allow a 'slot' parameter, and only pass the
## indicated arguments. 
qconnect <- function(x, signal, handler, user.data)
{
  stopifnot(is(x, "QObject"))
  stopifnot(is.function(handler))
  signal <- as.character(signal)
  has.user.data <- !missing(user.data)
  if (!has.user.data)
    user.data <- NULL
  nargs <- length(formals(handler))
  if (has.user.data && !nargs)
    stop("if 'user.data' specified, handler must take at least one argument")
  signal <- qresolveSignature(x, signal, "signal")
  .Call(qt_qconnect, x, signal, handler, user.data, has.user.data)
}
