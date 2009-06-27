qtimer <- function(delay, handler) {
  timer <- .Call(qt_qtimer)
  qconnect(timer, "timeout", handler)
  timer$interval <- delay
  timer
}
