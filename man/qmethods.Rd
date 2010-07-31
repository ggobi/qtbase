\name{qmethods}
\alias{qmethods}
\alias{qproperties}

\title{
  Introspect methods and properties
}
\description{
  There are two types of exposed class members: methods and, for
  \code{QObject} derivatives, properties. These functions
  return \code{data.frame} objects describing methods and properties.
}
\usage{
qmethods(x)
qproperties(x)
}
\arguments{
  \item{x}{
    Object of class \code{RQtClass} for \code{qmethods},
    a \code{QObject} instance for \code{qproperties}
  }
}
\value{
  For \code{qmethods}: a \code{data.frame} with
  columns for method \code{name}, \code{return} type name, \code{signature}
  string, and whether the method is \code{static} and/or \code{protected}.

  For \code{qproperties}: a \code{data.frame} with columns
  for \code{type} name, and whether the property is \code{readable}
  and/or \code{writeble}. The property names are stored in the row names.
}
\author{
  Michael Lawrence
}

\examples{
qmethods(Qt$QObject)
qproperties(Qt$QWidget())
}
