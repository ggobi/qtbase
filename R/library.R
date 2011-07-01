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
## the same name. We want to avoid syntax like Qanviz$Qanviz$Layer, so
## the top namespace is implied. Qt itself is of course an exception.
qlibrary <- function(lib, namespace = deparse(substitute(lib)),
                     restrictToNamespace = FALSE)
{
  name <- tolower(deparse(substitute(lib)))
  attr(lib, "ns") <- namespace
  if (is.null(attr(lib, "name")))
    attr(lib, "name") <- name
  class(lib) <- c("RQtLibrary", "environment")
  classes <- qclasses(lib)
  hidden <- substring(classes, 1, 1) == "."
  classes[hidden] <- substring(classes[hidden], 2, nchar(classes[hidden]))
  if (!is.null(namespace)) { # remove the implied namespace
    prefix <- paste("^", sub("^\\.", "", namespace), "::", sep = "")
    if (restrictToNamespace) {
      inNS <- grep(prefix, classes)
      classes <- classes[inNS]
      hidden <- hidden[inNS]
    }
    names(classes) <- sub(prefix, "", classes)
    names(classes)[hidden] <- paste(".", names(classes)[hidden], sep = "")
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
  ns <- sub("\\.?(.*?)::.*", "\\1", names(classes)[hasPrefix])
  if (length(ns) && !is.null(namespace))
    ns <- paste(namespace, ns, sep = "::")
  uns <- setdiff(ns, classes)
  hiddenNS <- !(uns %in% ns[!hidden[hasPrefix]])
  uns[hiddenNS] <- paste(".", uns[hiddenNS], sep = "")
  for (nsi in uns) {
    env <- new.env()
    attributes(env) <- attributes(lib)
    assign(sub(paste("^", namespace, "::", sep = ""), "", nsi),
           qlibrary(env, nsi, FALSE), lib)
  }
  lib
}

## Usually, one just uses '$' to lookup classes; this will map a full
## class name through nested namespaces and internal classes.
qclassForName <- function(name, lib) {
  classes <- vector("list", length(name))
  if (is.null(lib))
    return(classes)
  .mget <- function(x) {
    objs <- mget(x, lib, ifnotfound = list(NULL))
    notfound <- unlist(lapply(objs, is.null))
    if (any(notfound))
      objs[notfound] <- mget(paste(".", x[notfound], sep = ""), lib,
                             ifnotfound = list(NULL))
    objs
  }
  if (is(lib, "RQtClass")) {
    namespace <- attr(lib, "name")
    lib <- attr(x, "env")
  }
  else namespace <- attr(lib, "ns")
  name <- sub(paste(namespace, "::", sep = ""), "", name)
  hasPrefix <- grepl("::", name)
  classes[!hasPrefix] <- .mget(name[!hasPrefix])
  ns <- factor(sub("^([^:])*::.*", "\\1", name[hasPrefix]))
  if (length(ns)) # unsplit() broken for zero-length factors
    classes[hasPrefix] <- unsplit(mapply(qclassForName,
                                         split(name[hasPrefix], ns),
                                         .mget(levels(ns))),
                                  ns)
  classes
}

print.RQtLibrary <- function(x, ...) {
  cat("Module '", attr(x, "name"), "' with ", length(ls(x)),
      " top-level classes\n", sep = "")
}

### Module object for Qt library.
Qt <- new.env()

