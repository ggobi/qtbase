## obtain a class object from a smoke module
qclass <- function(x, name) {
  structure(function(...) qinvokeStatic(x, name, ...), name = name, smoke = x,
            class = "RQtClass")
}

## invoke a static method
"$.RQtClass" <- function(x, name) {
  function(...)
    qinvokeStatic(x, name, ...)
}
