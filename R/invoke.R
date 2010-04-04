qinvoke <- function(x, method, ...) {
  .Call(qt_qinvoke, x, method, FALSE, list(...))
}

qinvokeStatic <- function(x, method, ...) {
  .Call(qt_qinvokeStatic, x, method, list(...))
}

qinvokeSuper <- function(x, method, ...) {
  .Call(qt_qinvoke, x, method, TRUE, list(...))
}
