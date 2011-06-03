\name{RQtClass}
\alias{RQtClass}

\title{Class Definitions}
\description{
  Each class defined by a library has a corresponding \code{RQtClass}
  object. Each object is a \code{function} and serves as the constructor
  for the class. Special behavior is defined, so that the function also
  behaves as a container of static methods and enumeration values
  defined by the class.
}
\details{
  Besides serving as a constructor, this object contains two types of
  members: static methods and enumeration values. Their names may be
  listed by calling \code{names} on the object. To access a member, use
  \code{$}, which supports auto-completion, thanks to the \code{names}
  method.

  Namespaces within a library are also represented as \code{RQtClass}
  objects, even though they are not actually classes.
}
\seealso{
  \code{\link{RQtLibrary}} (container of class objects),
  \code{\link{RQtObject}} (an instance of a class)
}
\examples{
## calling a constructor
widget <- Qt$QWidget()
## calling a static method
Qt$QWidget$tr("hello world")
## access an enum value within the 'Qt' namespace
Qt$Qt$AbsoluteSize
}
\author{Michael Lawrence}
