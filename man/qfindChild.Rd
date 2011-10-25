\name{qfindChild}
\alias{qfindChild}

\title{ Find child by name }
\description{
  Finds a child \code{QObject} by name. Mirrors a function in Qt
  that is inaccessible via the ordinary interface because it is a
  template.
}
\usage{
qfindChild(x, name)
}

\arguments{
  \item{x}{
    The parent \code{QObject}
  }
  \item{name}{
    The name of the child
  }
}
\details{
  This is particularly useful when working with QtDesigner, where
  objects in the UI file are named.
}
\value{
  The child \code{QObject}
}
\author{
Michael Lawrence
}

\examples{
parent <- Qt$QObject()
child <- Qt$QObject(parent)
child$objectName <- "foo"
qfindChild(parent, "foo")
}
