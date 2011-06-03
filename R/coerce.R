### Explicit coercion functions for translating some simple Qt data
### structures to and from native R objects, like vectors and matrices.

## These are all from C++ to R. For the other direction, see constructors.R.

as.matrix.QRectF <- function(x, ...) .Call("qt_coerce_QRectF", x, PACKAGE="qtbase")
as.matrix.QRect <- function(x, ...) .Call("qt_coerce_QRect", x, PACKAGE="qtbase")

as.vector.QPointF <- function(x, mode) as.vector(as.double(x), mode)
as.double.QPointF <- function(x, ...) .Call("qt_coerce_QPointF", x, PACKAGE="qtbase")
as.vector.QPoint <- function(x, mode) as.vector(as.integer(x), mode)
as.integer.QPoint <- function(x, ...) .Call("qt_coerce_QPoint", x, PACKAGE="qtbase")
## so that as.numeric() works in either case
as.double.QPoint <- function(x, ...) as.numeric(as.integer(x))

as.vector.QSizeF <- function(x, mode) as.vector(as.double(x), mode)
as.double.QSizeF <- function(x, ...) .Call("qt_coerce_QSizeF", x, PACKAGE="qtbase")
as.vector.QSize <- function(x, mode) as.vector(as.integer(x), mode)
as.integer.QSize <- function(x, ...) .Call("qt_coerce_QSize", x, PACKAGE="qtbase")
as.double.QSize <- function(x, ...) as.numeric(as.integer(x))

as.matrix.QPolygon <- function(x, ...) .Call("qt_coerce_QPolygon", x, PACKAGE="qtbase")
as.matrix.QPolygonF <- function(x, ...) .Call("qt_coerce_QPolygonF", x, PACKAGE="qtbase")

as.matrix.QTransform <- function(x, ...) .Call("qt_coerce_QTransform", x, PACKAGE="qtbase")

qcol2rgb <- as.matrix.QColor <- function(x, ...) .Call("qt_coerce_QColor", x, PACKAGE="qtbase")

as.character.QChar <- function(x, ...) .Call("qt_coerce_QChar", x, PACKAGE="qtbase")

as.list.QItemSelection <- function(x, ...) .Call("qt_coerce_QItemSelection", x, PACKAGE="qtbase")

as.list.QTestEventList <- function(x, ...) .Call("qt_coerce_QTestEventList", x, PACKAGE="qtbase")
as.list.QSignalSpy <- function(x, ...) .Call("qt_coerce_QSignalSpy", x, PACKAGE="qtbase")

as.QImage <- function(x, ...) UseMethod("as.QImage")
as.QImage.matrix <- function(x) {
  if (!is.matrix(x) || !is.integer(x) || (nrow(x) != 4 && nrow(x) != 3))
    rgb <- col2rgb(x, TRUE)
  else rgb <- x
  Qt$QImage(as.raw(rgb), ncol(x), nrow(x), ncol(x) * nrow(rgb),
            if (nrow(rgb) == 3) Qt$QImage$Format_RGB888
            else Qt$QImage$Format_ARGB32)
}
as.QImage.raster <- function(x) as.QImage.matrix(x)
