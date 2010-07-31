\name{RQtLibrary}
\alias{RQtLibrary}
\alias{Qt}

\title{Library object}
\description{
  Every library bound by Smoke is represented by a top-level library
  object, which is an environment containing an \code{\link{RQtClass}}
  object for each class defined by the library.
}
\details{
  Since \code{RQtLibrary} is essentially a plain old \code{environment}
  object, the conventional \code{environment} API behaves as
  expected. List the names of the available classes with \code{\link{ls}} and
  retrieve a definition with \code{$} or \code{get}, for example.
}
\seealso{
  \code{\link{RQtClass}}, which describes a class
}
\examples{
ls(Qt)
Qt$QWidget
}
\author{Michael Lawrence}
