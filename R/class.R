### Classes are functions (constructors) with static methods in an environment

## Obviously, we would like each class env to inherit from the parent
## class env, but this is not possible when there are multiple parents.
## Options:
## 1) Just populate the environment with all symbols
## 2) Create a special type of environment using object tables
## 3) Use first class as parent, copy the others

## The last option not very elegant, the second option is complicated,
## and the first option *might* be inefficient -- need to time it.
## Ok, for QWidget it takes 0.005 seconds. Worth it? Nope. Many simple
## optimizations possible if needed.

## obtain a class object from a smoke module and a name
qclass <- function(x, name) {
  env <- new.env()
  cl <- structure(function(...) qinvokeStatic(x, name, ...), name = name,
                  smoke = qsmoke(x), env = env, class = "RQtClass")
  methods <- qmethods(cl)
  methods <- subset(methods, !duplicated(name) & static & !protected)
  lapply(methods$name, function(name) {
    assign(name, function(...) qinvokeStatic(x, name, ...), env)
  })
  cl
}

## invoke a static method
"$.RQtClass" <- function(x, name) {
  function(...)
    qinvokeStatic(x, name, ...)
}

qmethods <- function(x) {
  stopifnot(is(x, "RQtClass"))
  methods <- .Call(qt_qmethods, x)
  names(methods) <- c("name", "return", "signature", "protected", "static")
  df <- as.data.frame(methods, stringsAsFactors=FALSE)
  df[!duplicated(df$signature),]
}

print.RQtClass <- function(x) {
  methods <- subset(qmethods(x), !protected)
  cat("Class '", attr(x, "name"), "' with ", nrow(methods), " public methods\n",
      sep = "")
}
