### RQtLibrary objects refer to a Smoke module describing
### a bound library (e.g. Qt).

### FIXME: Think about using some S4 here

## Get available Smoke modules
qsmokes <- function() {
  .Call(qt_qsmokes)
}

qsmoke <- function(x) {
  nm <- attr(x, "name")
  smoke <- qsmokes()[[nm]]
  if (is.null(smoke))
    stop("Smoke module for library '", nm, "' not found")
  smoke
}
  
## Get classes in library
qclasses <- function(x) {
  .Call(qt_qclasses, qsmoke(x))
}

qlibrary <- function(lib) {
  attr(lib, "name") <- tolower(deparse(substitute(lib)))
  class(lib) <- c("RQtLibrary", "environment")
  lapply(qclasses(lib), function(className) {
    getClass <- function() {
      class <- qsmokeClass(lib, className)
      rm(list = className, envir = lib)
      assign(className, class, lib) ## cache for further use
      lockBinding(className, lib)
      class
    }
    makeActiveBinding(className, getClass, lib)
  })
  lib
}

print.RQtLibrary <- function(x) {
  cat("Module '", attr(x, "name"), "' with ", length(ls(x)), " classes\n",
      sep = "")
}

### Module object for Qt library.
Qt <- new.env()

