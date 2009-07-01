qmethods <- function(x) {
  stopifnot(is(x, "QObject"))
  methods <- .Call(qt_qmethods, x)
  methods[[1]] <- c("method", "signal", "slot", "constructor")[methods[[1]] + 1]
  methods <- c(list(sub("\\(.*", "", methods[[2]])), methods)
  names(methods) <- c("name", "type", "signature", "return", "nargs")
  as.data.frame(methods, stringsAsFactors=FALSE, row.names = methods$signature)
}

qnormalizedSignature <- function(x) {
  .Call(qt_qnormalizedSignature, as.character(x))
}

qresolveSignature <- function(x, sig, type, nargs) {
  ## The user can specify a signal by name or by signature
  methods <- qmethods(x)
  if (!missing(type))
    methods <- subset(methods, type == type)
  sigs <- as.character(methods$signature)
  if (!length(grep("\\(", sig))) {
    sigs <- sigs[grep(paste("^", sig, "\\(", sep = ""), sigs)]
    if (length(sigs) > 1) {
      argmatch <- (if (missing(nargs)) 0 else nargs) == methods[sigs, "nargs"]
      if (sum(argmatch) == 0) # just take first, will fail below
        argmatch[1] <- TRUE
      if (sum(argmatch) > 1)
        stop("ambiguous method selection: ", paste(sigs, collapse=", "))
      sigs <- sigs[argmatch]
    }
    if (!length(sigs))
      stop("method does not exist")
    sig <- sigs[1]
  } else {
    sig <- qnormalizedSignature(sig)
    if (!(sig %in% sigs))
      stop("method does not exist")
  }
  ## slots cannot have default arguments, usually signals are multiplied
  if (!missing(nargs) && methods[sig, "nargs"] != nargs)
    stop("number of arguments does not match method signature")
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
    c(rownames(qproperties(x)), unique(qmethods(x)$name))
}

`$.QObject` <- function(x, name) {
  val <- try(qproperty(x, name), silent=TRUE)
  if (!inherits(val, "try-error"))
    val
  else if (name %in% qmethods(x)$name)
    function(...) qinvoke(x, name, ...)
  else {
    msg <- gsub("\n *", "", sub("^[^:]* : *", "", val))
    stop("Failed to get property '", name, "' (", msg, "), ",
         "while a dynamic method of that name does not exist")
  }
}

`$<-.QObject` <- function(x, name, value) {
  qproperty(x, name) <- value
  x
}
