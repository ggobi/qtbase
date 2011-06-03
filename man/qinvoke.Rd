\name{qinvoke}
\alias{qinvoke}
\alias{qinvokeStatic}
\title{
  Invoke a method
}
\description{
  These functions invoke a method on an object or class. Usually, one
  does not call these functions directly but uses the \code{$}
  short-cut, instead. 
}
\usage{
qinvoke(x, method, ...)
qinvokeStatic(x, method, ...)
}
\arguments{
  \item{x}{
    The object or class with the method
  }
  \item{method}{
    The name of the method 
  }
  \item{\dots}{
    Arguments to pass to the method
  }
}
\details{
  Perhaps the only reason to use one of these functions directly is in the
  context of functional iteration, e.g., when calling the same method on
  every object in a list with \code{lapply}.
}
\value{
  The return value of the method
}
\author{
  Michael Lawrence
}

\examples{
widgets <- replicate(length(letters), Qt$QWidget())
mapply(qinvoke, widgets, "setWindowTitle", letters)
}
