\name{DataFrameModel}
\alias{qdataFrameModel}
\title{DataFrameModel}
\usage{qdataFrameModel(df, parent=NULL, useRoles=FALSE,
editable=character(), ...)
qdataFrame(model) <- value
qdataFrame(model)
}
\description{The \code{qdataFrameModel} function creates a
\code{DataFrameModel}, an implementation of
\code{QAbstractItemModel} using a \code{data.frame}. This makes it
easy and fast to display and edit a \code{data.frame} in a
\code{QTableView} or any other derivative of
\code{QAbstractItemView}. The \code{qdataFrame} and
\code{qdataFrame<-} functions allow one to get and set the
\code{data.frame} underlying the model after construction.}
\details{\code{qdataFrameModel}: While a simple data.frame can be displayed as a textual table,
fancier tables require multiple data columns mapped to a single
model column, each playing a separate 'role'. To specify
additional roles, pass \code{useRoles = TRUE}. A role may be any
string; those used by Qt are listed in the \code{Qt::ItemDataRole}
enumeration. The \code{display} and \code{edit} roles are reserved
(see below). See the documentation of the
\code{QStyledItemDelegate} class for its expected data types for
each role.

A simple way to encode this is in the column name, syntax:
\code{[.headerName1][.headerName2][.etc].role}.
Examples:
\itemize{
\item{\code{.carColor.background} (background color for carColor column)}
\item{\code{.foreground} (foreground color for all columns)}
\item{\code{.firstName.lastName.font} (special font for first and last
name columns)}
}

The set of model columns is derived from the unique header names.
Display-role columns are those not prefixed by a period. If the
column name in the data matches a string in the \code{editable}
argument, the data is used for both the edit and display roles.

}
\note{Calling the \code{headerData} method on
\code{DataFrameModel} from R will not yield the expected result,
because Smoke does not know of DataFrameModel and thus misses the
override of the non-pure virtual.  We can special case this if
need-be.}
\value{\code{qdataFrameModel}: An instance of C++ \code{DataFrameModel}

}
\author{Michael Lawrence}
\arguments{\item{df}{The \code{data.frame} that provides the data of the model}
\item{...}{Extra arguments passed to \code{qdataFrame<-},
which actually loads the \code{data.frame} into the model.}
\item{model}{\code{DataFrameModel} instance}
\item{parent}{The parent \code{QObject}}
\item{useRoles}{Whether to interpret column names as indicating
alternative roles; see details.}
\item{editable}{Character vector of column names in the
\code{data.frame} that should be editable}
\item{value}{A \code{data.frame} that provides the data of the model}
}
\alias{qdataFrame<-}
\alias{qdataFrame}

