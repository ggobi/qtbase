\name{RQtObject}
\alias{RQtObject}
\alias{qcol2rgb}
\alias{\%>>\%}
\alias{\%<<\%}
\alias{RQtObject-class}

\title{Instance of a Class}
\description{
  Represents an instance of a class, as a special type of
  \code{environment}. In this case, the members are methods and, for
  \code{QObject}-derivatives, properties.
}
\details{
  The ordinary \code{environment} API is supported, with restrictions on
  assignments. Retrieve objects with \code{$} or \code{get}. Replacing a
  method is not supported. For \code{QObject} instances, the members
  include the properties, which may be set through assignment, e.g.,
  with the \code{$<-} method.

  C++ operators are also supported, with the familiar syntax. Most C++
  operators have obvious R equivalents. Two exceptions are the \code{>>}
  and \code{<<} operators, which are defined as the infix functions
  \code{\%>>\%} and \code{\%<<\%} in R.

  Some classes in the Qt API are light-weight, and their instances are
  usually treated as values. For some of the most prevalent of these
  types, we define coercion methods to convenient R equivalents. Below,
  we list the R type corresponding to each Qt type. Coercion methods are
  defined for the conventional generic for the type,
  e.g., \code{as.integer} for integers. For those types with a variant
  ending in \code{F}, like \code{QSizeF}, the \code{F} variant yields
  double values in R, while \code{QSize} would be integer.

  \describe{
    \item{\code{QRect(F)}}{=> \code{matrix}}
    \item{\code{QPoint(F)}}{=> \code{vector}, \code{integer} (\code{double})}
    \item{\code{QSize(F)}}{=> \code{vector}, \code{integer} (\code{double})}
    \item{\code{QPolygon(F)}}{=> \code{matrix}}
    \item{\code{QTransform}}{=> \code{matrix}}
    \item{\code{QColor}}{=> \code{matrix}}
  }
}
\seealso{
  \code{\link{RQtClass}} (for constructing instances)
}
\examples{
widget <- Qt$QWidget()
widget$setWindowTitle("Hello World")
widget$windowTitle # access a property
widget$windowTitle <- "Hello Again"

size <- qsize(2L, 3L)
as.integer(size * 2) # operators work too
as.integer(size) * 2
}
\author{Michael Lawrence}
