

qshow <- function(x)
{
    .Call(qt_qshow, x)
}

qupdate <- function(x)
{
    .Call(qt_qupdate, x)
}

print.QWidget <- function(x, ...)
{
    qshow(x)
    invisible(x)
}

qclose <- function(x) 
{
    .Call(qt_qclose, x)
}

qlower <- function(x) 
{
    .Call(qt_qlower, x)
}

qraise <- function(x) 
{
    .Call(qt_qraise, x)
}

qparent <- function(x)
{
    .Call(qt_qparent, x)
}

qsetParent <- function(x, w)
{
    .Call(qt_qsetParent, x, w)
}

qresize <- function(x, w, h)
{
    .Call(qt_qresize, x, w, h)
}

qheight <- function(x)
{
    .Call(qt_qheight, x)
}

qwidth <- function(x)
{
    .Call(qt_qwidth, x)
}

qsetMinimumSize <- function(x, w, h)
{
    .Call(qt_qsetMinimumSize, x, w, h)
}

qsetExpanding <- function(x, vertical = NULL, horizontal = NULL)
{
    .Call(qt_setExpanding, x, vertical, horizontal)
}

qsetContentsMargins <- function(x, left, top, right, bottom)
{
    if (inherits(x, "QLayout"))
        .Call(qt_qsetContentsMarginsLayout, x, left, top, right, bottom)
    else
        .Call(qt_qsetContentsMargins, x, left, top, right, bottom)
}

qisEnabled <- function(x)
{
    .Call(qt_qisEnabled, x)
}

qsetEnabled <- function(x, flag)
{
    .Call(qt_qsetEnabled, x, flag)
}

qwindowTitle <- function(x)
{
    .Call(qt_qwindowTitle, x)
}

qsetWindowTitle <- function(x, title)
{
    .Call(qt_qsetWindowTitle, x, title)
}

qrender <- function(x, file)
{
    .Call(qt_qrender, x, as.character(file)[1])
}

qrenderGraphicsView <- function(x)
{
    .Call(qt_qrenderGraphicsView, x)
}

qrenderToPixmap <- function(x, file)
{
    .Call(qt_qrenderToPixmap, x, as.character(file)[1])
}

qrenderToSVG <- function(x, file)
{
    .Call(qt_qrenderToSVG, x, as.character(file)[1])
}

qprint <- function(x, view = FALSE)
{
    if (view) .Call(renderViewWidget, x)
    else .Call(renderWidget, x, NULL)
    invisible()
}


