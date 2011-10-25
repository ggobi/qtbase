\name{qmocMethods}
\alias{qmocMethods}
\alias{qsignals}
\alias{qslots}

\title{ List MOC methods }
\description{
  Access information describing the Qt Meta Object Compiler
  (MOC) methods defined for a class. These are typically signals and
  slots, although arbitrary methods/constructors are also supported.
}
\usage{
qmocMethods(x)
qsignals(x)
qslots(x)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{
    A \code{QObject} object or derived class (as an \code{RQtClass}).
  }
}
\value{
  For \code{qmocMethods}, a \code{data.frame} with columns for the
  method \code{name}, \code{type} (\dQuote{signal}, \dQuote{slot}, etc),
  \code{signature}, \code{return} type, and \code{nargs} (argument
  count).

  \code{qsignals} returns a similar \code{data.frame} containing only
  signal methods and the columns \code{name} and \code{signature}.

  \code{qslots} returns a similar \code{data.frame} containing only
  slot methods and the columns \code{name}, \code{signature} and \code{return}.
}
\author{
  Michael Lawrence
}

\seealso{
  \code{\link{qsetSignal}}, \code{\link{qsetSlot}} for defining new MOC
  methods on user classes
}

\examples{
qmocMethods(Qt$QWidget)
widget <- Qt$QWidget()
qsignals(widget)
}
