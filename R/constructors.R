## constructors of R-level Qt types

## In RGtk2, there were small C structures that the programmer was
## expected to initialize in-line, without calling a constructor. This
## meant that we did not have a constructor to call from R, so the
## user provided an R-level structure (often a list) that would be
## converted by the wrappers. Qt also has small structures that are
## meant to be manipulated as values. These often have natural R
## counterparts (QString -> character, QPoint -> length two integer
## vector, QMatrix -> matrix, etc). But as these are classes, they
## have constructors. However, these constructors are not always
## convenient for casting the raw R equivalent to the normalized
## representation (in the simplest case just adding a class attribute
## indicating the Qt type). Such casting is at least currently
## required for interacting with Qt, as it keeps the interface simple
## and validation is generally good practice anyway.

## These constructors could be wrappers for the C++ constructor, or
## they could simply reshape the data purely within R. The former
## approach might mean less code/logic duplication, as the
## construction would only happen in C++. However, casting directly in
## R, the latter approach, likely involves less overhead, as these are
## simple operations, and, more importantly, it provides a concise
## specification of the data structure, expressed in R code.

qrect <- function(x0, y0, x1, y1) {
  if (length(x0) == 2) {
    x1 <- x0[2]
    x0 <- x0[1]
  }
  if (length(y0) == 2) {
    y1 <-  y0[2]
    y0 <-  y0[1]
  }
  matrix(as.numeric(c(x0, x1, y0, y1)), 2, 2)
}

qpoint <- function(x, y) {
  if (length(x) == 2)
    p <- x
  else p <- c(x, y)
  as.numeric(p)
}

qsize <- function(width, height) {
  if (length(width) == 2)
    s <- width
  else s <- c(width, height)
  as.numeric(s)
}

qfont <-
  function(family = "Arial",
           pointsize = -1L,
           weight = -1L,
           italic = FALSE, ...)
{
  list(family = family,
       pointsize = pointsize,
       weight = weight,
       italic = italic)
}

qcolor <- function(red = 0, green = 0, blue = 0, alpha = 255)
{
    if (is.character(red) || is.matrix(red))
    {
        rgbvals <-
            if (is.character(red)) col2rgb(red, alpha = TRUE)[,1]
        else
            red[,1]
        red <- rgbvals["red"]
        green <- rgbvals["green"]
        blue <- rgbvals["blue"]
        if (missing(alpha)) alpha <- rgbvals["alpha"]
    }
    t(as.integer(c(red, green, blue, alpha)))
}
