\name{constructors}
\alias{qbrush}
\alias{qcolor}
\alias{qfont}
\alias{qpen}
\alias{qpoint}
\alias{qpolygon}
\alias{qrect}
\alias{qsize}
\alias{qtransform}

\title{Convenience constructors}
\description{
  These functions construct some simple Qt helper objects. They offer
  convenience beyond that of the native constructor, largely because
  they might coerce from an R equivalent to a Qt object. Also, unlike
  C++, argument skipping is supported.
}
\usage{
qrect(x0, y0, x1, y1)
qpoint(x, y)
qsize(width, height)
qpolygon(x = NULL, y = NULL)
qfont(family = "Arial", pointsize = -1L, weight = -1L, italic = FALSE)
qpen(brush = qbrush(), width = 0L, style = Qt$Qt$SolidLine,
                 cap = Qt$Qt$SquareCap, join = Qt$Qt$BevelJoin)
qbrush(color = qcolor(), style = Qt$Qt$SolidPattern)
qcolor(red = 0, green = 0, blue = 0, alpha = 255)
qtransform(m11 = 1.0, m12 = 0.0, m13 = 0.0, m21 = 0.0, m22 = 1.0,
           m23 = 0.0, m31 = 0.0, m32 = 0.0, m33 = 1.0)
}
\arguments{
  \item{x0}{Top-left X coordinate. If a vector of length two, taken as
    \code{x0} and \code{x1}. If a 2x2 \code{matrix}, the columns
    represent the X and Y positions. If missing, a "null" rectangle is
    returned.}
  \item{y0}{Top-left Y coordinate. If a vector of length two, taken as
    \code{y0} and \code{y1}.}
  \item{x1}{Bottom-right X coordinate.}
  \item{y1}{Bottom-right Y coordinate.}
  \item{x}{X coordinate. If a vector of length two, taken as \code{x}
    and \code{y}. For \code{qpolygon}, passed to \code{\link{xy.coords}}.}
  \item{y}{Y coordinate. For \code{qpolygon}, passed
    to \code{\link{xy.coords}}.}
  \item{width}{Width for the size. If a vector of length two, taken
    as \code{width} and \code{height}. If missing, a "null" size is
    returned. For \code{qpen}, width of the line.}
  \item{height}{Height for the size.}
  \item{family}{Font family name.}
  \item{pointsize}{Font point size.}
  \item{weight}{Font weight (bold).}
  \item{italic}{Whether font is italicized.}
  \item{brush}{Something coercible to a \code{QBrush} object
    via \code{qbrush(brush)}.}
  \item{style}{Line dashing for \code{qpen}, pattern type for \code{qbrush}.}
  \item{cap}{Line cap style.}
  \item{join}{Line join style.}
  \item{color}{Something coercible to a \code{QColor} via \code{qcolor(color)}.}
  \item{red}{Red component, 0-255. If a string, taken as color name and
    converted. If a matrix, taken as output of \code{\link{col2rgb}}.}
  \item{green}{Green component, 0-255.}
  \item{blue}{Blue component, 0-255.}
  \item{alpha}{Alpha component, 0-255.}
  \item{m11}{Horizontal scaling factor}
  \item{m12}{Vertical shearing factor}
  \item{m13}{Horizontal projection factor}
  \item{m21}{Horizontal shearing factor}
  \item{m22}{Vertical scaling factor}
  \item{m23}{Vertical projection factor}
  \item{m31}{Horizontal translation factor}
  \item{m32}{Vertical translation factor}
  \item{m33}{Division factor}
}
\value{
  An instance, \code{\link{RQtObject}}
}
\details{
  For structures with a \code{F} variant, like \code{QSizeF}, the
  variant is chosen based on the type of the input. If the values are
  double, a \code{QSizeF} instance is returned, otherwise an instance of
  \code{QSize}.
}
\examples{
## notice the coercion chaining:
redColor <- qcolor("red")
redBrush <- qbrush("red")
redPen <- qpen("red")
blackPen <- qpen()

## Qt's constructor uses width/height, not x1/y1
rect <- qrect(as.matrix(Qt$QRectF(1, 1, 5, 5)))
as.matrix(rect)

## Creates 'QRect' not 'QRectF'; integer input
qrect(range(1:10), range(2:5))

qsize(as.vector(qpoint(5, 5)))

mono_it <- qfont("monospace", italic = TRUE)
}
\author{Michael Lawrence, Deepayan Sarkar}
