\name{qtimer}
\alias{qtimer}
\title{
  Register a timer task
}
\description{
  A convenience function for creating a \code{QTimer} object and
  registering and R handler that is fired after a specified delay. The
  timer continues to fire until it is stopped. This does not work yet.
}
\usage{
qtimer(delay, handler)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{delay}{
    The delay, in milliseconds.
  }
  \item{handler}{
    The R function to fire after the delay. 
  }
}
\value{
  A \code{QTimer} object. To stop it, call the \code{stop} method.
}
\author{
  Michael Lawrence
}

\examples{
timer <- qtimer(2000, function() {
  print("time out")
})
timer$singleShot <- TRUE
timer$start()
}