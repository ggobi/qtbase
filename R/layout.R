
qlayout <- function(x = NULL)
{
    .Call(qt_qlayout, x)
}

qsetLayout <- function(x, layout)
{
    .Call(qt_qsetLayout, x, layout)
}

qsetSpacing <- function(x, spacing)
{
    .Call(qt_qsetSpacing, x, spacing)
}

qsetVerticalSpacing <- function(x, spacing)
{
    .Call(qt_qsetVerticalSpacing, x, spacing)
}

qsetHorizontalSpacing <- function(x, spacing)
{
    .Call(qt_qsetHorizontalSpacing, x, spacing)
}

qaddWidgetToLayout <- function(x, widget, row, column, nrow = 1L, ncolumn = 1L)
{
    stopifnot(inherits(widget, "QWidget"))
    .Call(qt_qaddWidgetToLayout, x, widget, row, column, nrow, ncolumn)
}

qremoveWidgetFromLayout <- function(x, widget)
{
    .Call(qt_qremoveWidgetFromLayout, x, widget)
}

qaddLayoutToLayout <- function(layout, row, column, nrow = 1L, ncolumn = 1L)
{
    .Call(qt_qaddLayoutToLayout, x, layout, row, column, nrow, ncolumn)
}

qcolumnCount <- function(x)
{
    .Call(qt_qcolumnCount, x)
}

qrowCount <- function(x)
{
    .Call(qt_qrowCount, x)
}

qdim <- function(x) 
{
    c(qrowCount(x), qcolumnCount(x))
}

