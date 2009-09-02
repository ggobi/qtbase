qinvoke <- function(x, method, ...) {
  invisible(.Call(qt_qinvoke, x, method, list(...)))
}

qinvokeStatic <- function(x, method, ...) {
  invisible(.Call(qt_qinvokeStatic, x, method, list(...)))
}
