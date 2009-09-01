qinvoke <- function(x, method, ...) {
  invisible(.Call(qt_qinvoke, x, method, list(...)))
}

qinvokeStatic <- function(x, class, method, ...) {
  invisible(.Call(qt_qinvokeStatic, x, class, method, list(...)))
}
