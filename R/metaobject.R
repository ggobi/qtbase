qmethods <- function(x) {
  stopifnot(is(x, "QObject"))
  methods <- .Call(qt_qmethods, x)
  methods[[1]] <- c("method", "signal", "slot", "constructor")[methods[[1]] + 1]
  names(methods) <- c("type", "signature", "return")
  as.data.frame(methods, row.names=sub("\\(.*", "", methods$signature))
}

qnormalizedSignature <- function(x) {
  .Call(qt_qnormalizedSignature, as.character(x))
}

qresolveSignature <- function(x, sig, type) {
  ## The user can specify a signal by name or by signature
  methods <- qmethods(x)
  if (!missing(type))
    methods <- subset(methods, type == type)
  sigs <- as.character(methods$signature)
  if (!length(grep("\\(", sig))) {
    sigs <- sigs[grep(paste("^", sig, "\\(", sep = ""), sigs)]
    if (length(sigs) > 1) {
      noargs <- nchar(sigs) == nchar(sig)+2
      if (!any(noargs))
        stop("ambiguous method selection: ", sigs)
      sigs <- sigs[noargs]
    }
    if (!length(sigs))
      stop("method does not exist")
    sig <- sigs[1]
  } else {
    sig <- qnormalizedSignature(sig)
    if (!(sig %in% sigs))
      stop("method does not exist")
  }
  sig
}

qproperties <- function(x) {
  stopifnot(is(x, "QObject"))
  props <- .Call(qt_qproperties, x)
  names(props) <- c("name", "type", "readable", "writable")
  name <- props$name
  props$name <- NULL
  as.data.frame(props, row.names=name)
}

names.QObject <- function(x) {
    c(rownames(qproperties(x)), unique(rownames(qmethods(x))))
}

`$.QObject` <- function(x, name) {
  if (name %in% rownames(qproperties(x)))
    qproperty(x, name)
  else function(...) qinvoke(x, name, ...)
}
`$<-.QObject` <- function(x, name, value) {
  qproperty(x, name) <- value
  x
}
