qtcon <- function(stream) UseMethod("qtcon")

qtcon.QIODevice <- function(stream, binary = FALSE) {
    stopifnot(is(stream, "QIODevice"))
    stopifnot(isTRUEorFALSE(binary))
    .Call("qt_qtcon_QIODevice", stream, binary, PACKAGE="qtbase")
}
