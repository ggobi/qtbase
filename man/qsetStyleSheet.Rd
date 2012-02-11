\name{qsetStyleSheet}
\alias{qsetStyleSheet}
\alias{qstyleSheet}
\title{
  Access style sheets
}
\description{
  Convenience functions for defining style sheets in Qt.
}
\usage{
qsetStyleSheet(..., what = "*", widget = NULL, append = TRUE)
qstyleSheet(widget = NULL)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{\dots}{
    Optionally named arguments specifying style sheet parameters.
  }
  \item{what}{
    The entity to which the parameters apply.
  }
  \item{widget}{
    Widget containing the style sheet. If \code{NULL}, it applies
    application-wide.
  }
  \item{append}{
    Whether the rules should be appended to the existing rules.
  }
}
\value{
  For \code{qstyleSheet}, the style sheet in textual form.
}
\author{
  Michael Lawrence
}
