### Explicit coercion functions for translating some simple Qt data
### structures to and from native R objects, like vectors and matrices.

## To R

as.matrix.QRectF <- function(x) .Call(qt_coerce_QRectF, x)
as.matrix.QRect <- function(x) .Call(qt_coerce_QRect, x)

as.vector.QPointF <- function(x) .Call(qt_coerce_QPointF, x)
as.vector.QPoint <- function(x) .Call(qt_coerce_QPoint, x)

as.vector.QSizeF <- function(x) .Call(qt_coerce_QSizeF, x)
as.vector.QSize <- function(x) .Call(qt_coerce_QSize, x)

as.matrix.QTransform <- function(x) .Call(qt_coerce_QTransform, x)

as.raw.QByteArray <- function(x) .Call(qt_coerce_QByteArray, x)
as.character.QByteArray <- function(x) rawToChar(as.raw.QByteArray(x))

qcol2rgb <- as.matrix.QColor <- function(x) .Call(qt_coerce_QColor, x)

## From R

## Take QTransform as an example.
## We want some symmetry with as.matrix(), i.e. a coercion. In C++,
## coercions are defined by constructors, but in R they are kept
## separate (see factor/as.factor, data.frame/as.data.frame). We
## probably want to avoid S4, again for consistency with the as.matrix
## S3 generic, so we define a new generic as.QTransform.

as.QTransform <- function(x, ...) UseGeneric("as.QTransform")
as.QTransform.matrix <- function(x) {
  stopifnot(is.numeric(x) && ncol(x) == 3L && nrow(x) == 3L)
  Qt$QTransform(x[1,1], x[2,1], x[3,1],
                x[1,2], x[2,2], x[3,2],
                x[1,3], x[2,3], x[3,3])
}
