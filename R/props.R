qproperty <- function(x, name) {
  stopifnot(is(x, "QObject"))
  props <- qproperties(x)
  if (!(name %in% rownames(props)))
    stop("property does not exist")
  if (!props[name,]$readable)
    stop("property is not readable")
  name <- as.character(name)
  .Call(qt_qproperty, x, name)
}

`qproperty<-` <- function(x, name, value) {
  stopifnot(is(x, "QObject"))
  props <- qproperties(x)
  if (!(name %in% rownames(props)))
    stop("property does not exist")
  if (!props[name,]$writable)
    stop("property is not writable")
  .Call(qt_qsetProperty, x, as.character(name), value)
}
