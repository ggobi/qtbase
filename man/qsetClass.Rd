\name{qsetClass}
\alias{qsetClass}
\alias{qsetMethod}
\alias{qsetSlot}
\alias{qsetProperty}
\alias{qsetSignal}
\alias{qsetRefClass}
\title{
  Define a Qt/C++ class in R
}
\description{
  C++ libraries like Qt often expect/require clients to extend classes
  in the API. \code{qsetClass} will define a class in terms of its name,
  parent (single inheritance) and constructor. Methods are added with
  with \code{qsetMethod}, \code{qsetSlot}, and \code{qsetSignal}. The
  \code{qsetProperty} function defines new
  properties. Calling \code{qsetRefClass} on an \code{RQtClass} creates
  a corresponding \dQuote{R5} reference class, thus unifying R/Qt
  classes with the \code{methods} package.
}
\usage{
qsetClass(name, parent, constructor = function(...) parent(...),
          where = topenv(parent.frame()))
qsetMethod(name, class, FUN,
           access = c("public", "protected", "private"))
qsetSlot(signature, class, FUN,
         access = c("public", "protected", "private"))
qsetSignal(signature, class,
           access = c("public", "protected", "private"))
qsetProperty(name, class, type = NULL, read = function() this[[.name]], 
    write = function(val) this[[.name]] <- val, notify = NULL, 
    constant = FALSE, final = FALSE, stored = TRUE, user = FALSE)
qsetRefClass(Class, where = topenv(parent.frame()), ...)
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
  \item{class, Class}{
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
  \item{signature}{
    The name and types of the arguments. If there are no arguments, and
    the no return value, this is just the name. Otherwise, provide the
    signature in C++ syntax, as in: \code{int myMethod(int, const
    char*)} for a method named \code{myMethod} that accepts two
    parameters, one an integer and one a string, and then returns an
    integer. We are essentially using C++ as a DSL for specifying
    signatures; the types must be C++ types, because this method is made
    available to external systems (like DBus and Javascript via
    QtWebKit).
  }
  \item{type}{The type name for the property}
  \item{read}{The function for reading (getting) the value of this
    property. By default, this assumes the property is stored as a
    field named of the form \code{.name}.}
  \item{write}{The function for writing (setting) the value of this
    property. By default, this assumes the property is stored as a
    field named of the form \code{.name}.}
  \item{notify}{The name of a previously defined signal that is emitted
    when the property is modified, or \code{NULL} for none.
  }
  \item{constant}{A hint to Qt that the property does not change; not
    actually enforced and rarely used.}
  \item{final}{A hint to Qt that the property is not to be overridden;
    not actually enforced and rarely used.
  }
  \item{stored}{A hint to Qt that the property is stored in the object,
    i.e., not dynamically computed. Could be helpful when
    serializing objects.
  }
  \item{user}{
    Whether the property is the primary user-facing property for this
    class. Like the current value for a scrollbar. Used when autowiring
    widgets to data models, as in \code{QDataWidgetMapper}.
  }
  \item{...}{Additional arguments to pass to \code{setRefClass}.}
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

  For \code{qsetMethod}, \code{qsetSlot}, \code{qsetSignal}, and
  \code{qsetProperty}, the name of the method/property (supports
  chaining).

  For \code{qsetRefClass}, the reference class generator object
  corresponding to the R/C++ class.
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
