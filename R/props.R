qproperty <- function(x, name) {
  .Call(qt_qproperty, x, name)
}

`qproperty<-` <- function(x, name, value) {
  .Call(qt_qsetProperty, x, name, value)
}
