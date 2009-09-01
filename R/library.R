### RQtLibrary objects refer to a Smoke module describing
### a bound library (e.g. Qt).

### FIXME: Think about using some S4 here

## Get available Smoke modules
qsmokes <- function() {
  .Call(qt_qsmokes)
}

## Get classes in Smoke module
qclasses <- function(x) {
  .Call(qt_qclasses, x)
}

qlibrary <- function(name) {
  structure(new.env(), name = name, class = "RQtLibrary")
}

qinit <- function(lib) {
  stopifnot(inherits(lib, "RQtLibrary"))
  nm <- attr(lib, "name")
  smoke <- qsmokes()[[nm]]
  if (is.null(smoke))
    stop("Smoke module for library '", nm, "' not found")
  lapply(qclasses(smoke), function(className) {
    getClass <- function() {
      class <- qclass(smoke, className)
      rm(list=className, envir=lib)
      assign(className, class, lib) ## cache for further use
      class
    }
    makeActiveBinding(className, getClass, lib)
  })
}

### Module object for Qt library.
Qt <- qlibrary("Qt")
