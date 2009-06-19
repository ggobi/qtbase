
qsetDeleteOnClose <- function(x, status = TRUE)
{
    .Call(qt_qsetDeleteOnClose, x, status)
}

