qinvoke <- function(x, method, ...) {
  invisible(.Call(qt_qinvoke, x, method, FALSE, list(...)))
}

qinvokeStatic <- function(x, method, ...) {
  invisible(.Call(qt_qinvokeStatic, x, method, list(...)))
}

qinvokeSuper <- function(x, method, ...) {
  invisible(.Call(qt_qinvoke, x, method, TRUE, list(...)))
}
