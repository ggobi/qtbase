### RQtLibrary objects refer to a Smoke module describing
### a bound library (e.g. Qt).

### FIXME: Think about using some S4 here

## Get available Smoke modules
qsmokes <- function() {
  .Call("qt_qsmokes", PACKAGE="qtbase")
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
  .Call("qt_qclasses", qsmoke(x), PACKAGE="qtbase")
}

## Many libraries define all of their classes within a namespace of
## the same name We want to avoid syntax like Qanviz$Qanviz$Layer, so
## the top namespace is implied. Qt itself is of course an exception.
qlibrary <- function(lib, namespace = deparse(substitute(lib))) {
  force(namespace)
  name <- tolower(deparse(substitute(lib)))
  if (is.null(attr(lib, "name")))
    attr(lib, "name") <- name
  class(lib) <- c("RQtLibrary", "environment")
  classes <- qclasses(lib)
  if (!is.null(namespace)) { # remove the implied namespace
    prefix <- paste("^", namespace, "::", sep = "")
    classes <- grep(prefix, classes, value=TRUE)
    names(classes) <- sub(prefix, "", classes)
  } else names(classes) <- classes
  hasPrefix <- grepl("::", names(classes))
  ## take care of non-namespaced/non-internal classes first
  lapply(names(classes[!hasPrefix]), function(classAlias) {
    getClass <- function() {
      class <- qsmokeClass(lib, classes[[classAlias]])
      rm(list = classAlias, envir = lib)
      assign(classAlias, class, lib) ## cache for further use
      lockBinding(classAlias, lib)
      class
    }
    makeActiveBinding(classAlias, getClass, lib)
  })
  ## now we need a separate environment for each namespace
  ns <- setdiff(sub("(.*)::.*", "\\1", classes[hasPrefix]), classes)
  for (nsi in ns) {
    env <- new.env()
    attributes(env) <- attributes(lib)
    assign(nsi, qlibrary(env, nsi), lib)
  }
  lib
}

print.RQtLibrary <- function(x, ...) {
  cat("Module '", attr(x, "name"), "' with ", length(ls(x)),
      " top-level classes\n", sep = "")
}

### Module object for Qt library.
Qt <- new.env()

