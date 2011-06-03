\name{qlibrary}
\alias{qlibrary}
\title{
  Populate a library object
}
\description{
  This function is only for use by packages that provide a library to
  R. It should be called in the \code{\link{.onLoad}} function of the
  package.
}
\usage{
qlibrary(lib, namespace = deparse(substitute(lib)))
}
\arguments{
  \item{lib}{
    An \code{environment} object that exists in the namespace of the
    package. The name of the object in the calling frame must match the
    name of the module in Smoke, unless the \code{name} attribute is set.
  }
  \item{namespace}{
    If not \code{NULL}, the implicit top-level namespace of the
    library. Many libraries have a top-level namespace, which usually
    should be ignored (implied) for convenience. Note that this is not
    the case for Qt, even though it does place many enumerations inside
    the \code{Qt} namespace.
  }
}
\value{
  The library object, but catching the return value is useless, since,
  as an \code{environment}, it was modified by reference.
}
\author{
  Michael Lawrence
}

\seealso{
  The vignette, once it exists, for creating packages based on qtbase
}

\examples{
## regenerate the Qt library object
Qt <- new.env()
qlibrary(Qt, NULL)
}
