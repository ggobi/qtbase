
qtabWidget <- function() 
{
    .Call(qt_qtabWidget)
}

qaddTab <- function(x, tab, label, index = 0L)
{
    if (missing(label))
    {
        if (index == 0)
            ## should be length(x) + 1 
            label <- qcurrentIndex(x) + 1
        else
            label <- index
    }
    .Call(qt_qaddTab, x, tab, as.character(label)[1], as.integer(index)[1])
}

qremoveTab <- function(x, index = qcurrentIndex(x))
{
    .Call(qt_qremoveTab, x, as.integer(index)[1])
}

qstackedWidget <- function()
{
    .Call(qt_qstackedWidget)
}

qaddWidgetToStack <- function(x, w, index)
{
    .Call(qt_qaddWidgetToStack, x, w, as.integer(index)[1])
}

qremoveWidgetFromStack <- function(x, index)
{
    .Call(qt_qremoveWidgetFromStack, x, as.integer(index)[1])
}

qcurrentIndex <- function(x)
{
    .Call(qt_qcurrentIndex, x)
}

qsetCurrentIndex <- function(x, index)
{
    .Call(qt_qsetCurrentIndex, x, as.integer(index)[1])
}
    
