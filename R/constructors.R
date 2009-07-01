## constructors of R-level Qt types

qrect <- function(x0, y0, x1, y1) {
  if (length(x0) == 2) {
    x1 <- x0[2]
    x0 <- x0[1]
  }
  if (length(y0) == 2) {
    y1 <-  y0[2]
    y0 <-  y0[1]
  }
  r <- matrix(as.numeric(c(x0, x1, y0, y1)), 2, 2)
  class(r) <- "QRectF"
  r
}

qpoint <- function(x, y) {
  p <- as.numeric(c(x, y))
  class(p) <- "QPointF"
  p
}

qsize <- function(width, height) {
  s <- as.numeric(c(width, height))
  class(s) <- "QSizeF"
  s
}

qfont <-
  function(family = "Arial",
           pointsize = -1L,
           weight = -1L,
           italic = FALSE, ...)
{
  f <- list(family = family,
            pointsize = pointsize,
            weight = weight,
            italic = italic)
  class(f) <- "QFont"
  f
}

qcolor <- function(red = 0, green = 0, blue = 0, alpha = 255) {
  col <- t(as.integer(c(red, green, blue, alpha)))
  class(col) <- "QColor"
  col
}
