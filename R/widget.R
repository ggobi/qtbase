### Conveniences for widgets

print.QWidget <- function(x, ...)
{
  x$show()
  invisible(x)
}
