## Holds some convenience wrappers for various objects

##' Get the dimensions of rectangles and rectangular objects
##'
##' @param x A rectangular object, like \code{QRect} or \code{QGraphicsItem}
##' @method dim QRectF
##' @rdname dim-methods
dim.QRectF <- function(x) c(x$width(), x$height())
##' @method dim QRect
##' @rdname dim-methods
dim.QRect <- dim.QRectF
##' @method dim QGraphicsScene
##' @rdname dim-methods
dim.QGraphicsScene <- function(x) dim(x$sceneRect)
##' @method dim QGraphicsItem
##' @rdname dim-methods
dim.QGraphicsItem <- function(x) dim(x$boundingRect)
##' @method dim QGraphicsView
##' @rdname dim-methods
dim.QGraphicsView <- function(x) dim(x$viewport()$rect)
