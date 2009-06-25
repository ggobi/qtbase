
## TODO: possibly allow a 'slot' parameter, and only pass the
## indicated arguments. 
qconnect <- function(x, signal, handler = NULL, user.data = NULL)
{
  stopifnot(is(x, "QObject"))
  stopifnot(is.function(handler))
  signal <- as.character(signal)
  has.user.data <- !missing(user.data)
  nargs <- length(formals(handler))
  if (has.user.data && !nargs)
    stop("if 'user.data' specified, handler must take at least one argument")
  
  ## The user can specify a signal by name or by signature
  methods <- qmethods(x)
  signals <- as.character(subset(methods, type == "signal")$signature)
  if (!length(grep("\\(", signal))) {
    signals <- signals[grep(paste("^", signal, "\\(", sep = ""), signals)]
    if (length(signals) > 1) {
      noargs <- nchar(signals) == nchar(signal)+2
      if (!any(noargs))
        stop("ambiguous signal selection: ", signals)
      signals <- signals[noargs]
    }
    if (!length(signals))
      stop("signal does not exist")
    signal <- signals[1]
  } else {
    signal <- qnormalizedSignature(signal)
    if (!(signal %in% signals))
      stop("signal does not exist")
  }
  
  .Call(qt_qconnect, x, signal, handler, user.data, has.user.data)
}

qdisconnect <- function(x, receiver = NULL)
{
  stopifnot(is(x, "QObject"))
  stopifnot(is.null(receiver) || is(receiver, "QObject"))
  .Call(qt_qdisconnect, x, receiver)
}
