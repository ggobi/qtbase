\name{qconnect}
\alias{qconnect}
\title{
  Connect a signal handler
}
\description{
  Like many other GUI toolkits, Qt supports connecting handler functions
  to signals, which are emitted in response to user
  actions. The \code{qconnect} function connects an R function to a
  signal on a Qt object.
}
\usage{
qconnect(x, signal, handler, user.data)
}
\arguments{
  \item{x}{
    The Qt object providing the signal
  }
  \item{signal}{
    The name of the signal
  }
  \item{handler}{
    The R function to handle the signal
  }
  \item{user.data}{
    Data to pass to the R function as the last parameter. If omitted, no
    such argument is passed to the handler.
  }
}
\details{
  In Qt, only other \code{QObject} instances can listen to a signal, so
  this function creates a dummy object that invokes the R function upon
  receipt of the signal.

  To disconnect a handler, call the \code{disconnect} method on the
  signal emitter \code{x}, passing it the returned dummy \code{QObject};
  see the example.
}

\value{
  A dummy \code{QObject} instance that is listening to the signal. Will
  become invalid when the signal emitter is invalidated.
}
\author{
  Michael Lawrence
}

\examples{
window <- Qt$QWidget()
button <- Qt$QPushButton(window)
listener <- qconnect(button, "pressed", function(x) print("hello world"))
button$disconnect(listener)
}
