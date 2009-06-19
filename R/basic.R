

qwidget <- function()
{
    .Call(qt_qwidget)
}

qpushButton <- function(label = "Button")
{
    .Call(qt_qpushButton, label)
}

qlabel <- function(label = "Label")
{
    .Call(qt_qlabel, label)
}

qlineEdit <- function(text = "")
{
    .Call(qt_qlineEdit, text)
}

qtext <- function(x)
{
    if (is(x, "QAbstractButton"))
        .Call(qt_qtextButton, x)
    else if (is(x, "QLabel"))
        .Call(qt_qtextLabel, x)
    else if (is(x, "QLineEdit"))
        .Call(qt_qtextLineEdit, x)
    else NULL
}

qsetText <- function(x, s = "")
{
    if (is(x, "QAbstractButton"))
        .Call(qt_qsetTextButton, x, s)
    else if (is(x, "QLabel"))
        .Call(qt_qsetTextLabel, x, s)
    else if (is(x, "QLineEdit"))
        .Call(qt_qsetTextLineEdit, x, s)
    else NULL
}

qcheckBox <- function(label)
{
    .Call(qt_qcheckBox, as.character(label)[1])
}

qisChecked <- function(x)
{
    if (is(x, "QAbstractButton"))
        .Call(qt_qisCheckedButton, x)
    else NULL
}

qsetChecked <- function(x, status)
{
    if (is(x, "QAbstractButton"))
        .Call(qt_qsetCheckedButton, x, as.integer(status)[1])
    else NULL
}


