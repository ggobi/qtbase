### Explicit coercion functions for translating some simple Qt data
### structures to and from native R objects, like vectors and matrices.

## These are all from C++ to R. For the other direction, see constructors.R.

as.matrix.QRectF <- function(x) .Call(qt_coerce_QRectF, x)
as.matrix.QRect <- function(x) .Call(qt_coerce_QRect, x)

as.vector.QPointF <- function(x) .Call(qt_coerce_QPointF, x)
as.vector.QPoint <- function(x) .Call(qt_coerce_QPoint, x)

as.vector.QSizeF <- function(x) .Call(qt_coerce_QSizeF, x)
as.vector.QSize <- function(x) .Call(qt_coerce_QSize, x)

as.matrix.QPolygon <- function(x) .Call(qt_coerce_QPolygon, x)
as.matrix.QPolygonF <- function(x) .Call(qt_coerce_QPolygonF, x)

as.matrix.QTransform <- function(x) .Call(qt_coerce_QTransform, x)

as.raw.QByteArray <- function(x) .Call(qt_coerce_QByteArray, x)
as.character.QByteArray <- function(x) rawToChar(as.raw.QByteArray(x))

qcol2rgb <- as.matrix.QColor <- function(x) .Call(qt_coerce_QColor, x)
