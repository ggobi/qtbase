\name{as.QImage}
\alias{as.QImage}
\alias{as.QImage.default}
\title{
  Coerce to QImage
}
\description{
  Coercion methods for converting R objects to QImage
}
\usage{
as.QImage(x, ...)
\S3method{as.QImage}{default}(x, ...)
}
\arguments{
  \item{x}{
    Object to coerce. By default, a \code{raster} object,
    or \code{matrix} as returned by \code{\link{col2rgb}} (or something
    coercible to one).
  }
  \item{\dots}{
    Arguments to pass to methods
  }
}

\value{
  A \code{QImage} object
}
\author{
  Michael Lawrence
}
