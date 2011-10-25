\name{qrTextFormattingDelegate}
\alias{qrTextFormattingDelegate}

\title{
  R-style Text Formatting Delegate
}
\description{
  Constructs an object of class \code{RTextFormattingDelegate}, which
  implements \code{QItemDelegate} for displaying text using the R
  formatting engine. This allows table and tree widgets (and any other
  widget that delegates to \code{ItemDelegate}) to correctly display
  \code{NA} values and to display numbers using
  the \code{scipen} and \code{digits} options.
}
\usage{
qrTextFormattingDelegate(parent = NULL)
}

\arguments{
  \item{parent}{
    The parent object, or \code{NULL} for none.
  }
}
\value{
  An \code{RTextFormattingDelegate} object
}
\author{
  Michael Lawrence
}

\examples{
data(mtcars)
model <- qdataFrameModel(mtcars)
view <- Qt$QTableView()
view$setModel(model)
delegate <- qrTextFormattingDelegate()
view$setItemDelegate(delegate)
view
}
