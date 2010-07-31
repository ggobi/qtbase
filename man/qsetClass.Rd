\name{qsetClass}
\alias{qsetClass}
\alias{qsetMethod}
\title{
  Define a C++ class in R
}
\description{
  C++ libraries like Qt often expect/require clients to extend classes
  in the API. \code{qsetClass} will define a class in terms of its name,
  parent (single inheritance) and constructor. Methods are added with
  with \code{\link{qsetMethod}}.
}
\usage{
qsetClass(name, parent, constructor = function(...) parent(...),
          where = topenv(parent.frame()))
qsetMethod(name, class, FUN,
           access = c("public", "protected", "private"), slot = FALSE)
}
\arguments{
  \item{name}{
    The name of the class or method.
  }
  \item{parent}{
    Object of \code{\link{RQtClass}} representing the parent. Only
    single inheritance is supported.
  }
  \item{constructor}{
    A function for constructing an instance. By default, arguments are
    passed to parent constructor. See details.
  }
  \item{where}{
    The environment in which to define the class. Behaves like
    \code{\link{setClass}}. Usually not specified.
  }
  \item{class}{
    Object of \code{\link{RQtClass}} on which to define the method.
  }
  \item{FUN}{
    The function that implements the method.
  }
  \item{access}{
    The access modifier; same meaning as in C++. \code{public} methods
    may be invoked from any context, \code{protected} only by methods of
    this class or a subclass, and \code{private} only by methods of this
    class.
  }
  \item{slot}{
    Whether to add the method to the Qt MOC metadata. Might be useful
    for implementing a DBus service. Currently unsupported. Email
    maintainer if interested.
  }
}
\details{
  The side-effect of \code{qsetClass} is that a \code{RQtClass} object
  for the new R class is assigned into the \code{where} argument.
  
  Within the scope of a method or constructor, the symbols are first
  resolved against the members of the class (including inherited
  members). The search then procedes to the enclosure of the R function,
  and on up the conventional search path.

  For chaining up, there is a special function named \code{super} that
  is defined differently for methods and constructors. Within a
  constructor, \code{super} will invoke the constructor of the super
  class, as in Java. For a method, the first argument passed to
  \code{super} should be the name of the method in the parent class to
  invoke (also similar to Java).
}
\value{
  For \code{qsetClass}, the \code{RQtClass} object (supports chaining
  with \code{qsetMethod}).

  For \code{qsetMethod}, the name of the method (supports chaining with
  \code{qinvoke}).
}
\author{
  Michael Lawrence
}

\examples{
e <- Qt$QLineEdit()

qsetClass("positiveValidator", Qt$QValidator)

qsetMethod("validate", positiveValidator, function(input, pos) {
  val <- suppressWarnings(as.integer(input))
  if (!is.na(val)) {
    if (val > 0)
      Qt$QValidator$Acceptable
    else Qt$QValidator$Invalid
  } else {
    if (input == "")
      Qt$QValidator$Acceptable
    else Qt$QValidator$Invalid
  }
})

v <- positiveValidator(e)
e$setValidator(v)
e$show()
}
