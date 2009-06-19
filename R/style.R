
## customizing appearance through stylesheets

construct.stylesheet <- function(..., what = "*")
{
    elts <- list(...)
    nms <-
        if (is.null(names(elts))) rep("", length(elts))
        else names(elts)
    ans <- character(length(elts))
    for (i in seq_along(elts))
    {
        ans[i] <-
            if (nms[i] == "") sprintf(" %s { %s } ", what, elts[i])
            else sprintf(" %s { %s : %s } ", what, nms[i], elts[i])
    }
    ans
}

qsetStyleSheet <- function(..., what = "*", widget = NULL, append = TRUE)
{
    style <- construct.stylesheet(..., what = "*")
    if (append) style <- c(qstyleSheet(widget), style)
    .Call(qt_qsetStyleSheet, widget, paste(style, collapse = "\n"))
}

qstyleSheet <- function(widget = NULL)
{
    .Call(qt_qstyleSheet, widget)
}

