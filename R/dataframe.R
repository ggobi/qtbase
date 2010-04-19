## Implement the QItemModel in QtCore using an R data.frame

## While a simple data.frame can be displayed as a textual table,
## fancier tables require multiple data columns mapped to a single
## model column, each playing a separate 'role'

## A simple way to encode this is in the column name, syntax:
## [.headerName1][.headerName2][.etc].role
## Example:
## .carColor.background (background color for carColor column)
## .foreground (foreground color for all columns)
## .firstName.lastName.font (special font for first and last name columns)
## The model columns are derived from the unique header names
## Display-role columns are those not prefixed by '.'

## NOTE: Calling the 'headerData' method on DataFrameModel from R will
## not yield the expected result, because Smoke does not know of
## DataFrameModel and thus misses the override of the non-pure virtual.
## We can special case this if need-be.

qdataFrameModel <- function(df, ...)
{
  model <- .Call(qt_qdataFrameModel)
  qsetDataFrame(model, df, ...)
  model
}

qsetDataFrame <- function(model, df)
{
  stopifnot(inherits(model, "DataFrameModel"))
  df <- as.data.frame(df)
  getHeaderNames <- function(x)
    strsplit(sub("^\\.", "", sub("\\.[^.]*$", "", x)), "\\.")
  header <- unique(unlist(getHeaderNames(colnames(df))))
  ## this order must match the order of the Qt::ItemDataRole enumeration
  roleNames <- c("display", "decoration", "edit", "toolTip", "statusTip",
                 "whatsThis", "font", "textAlignment", "background",
                 "backgroundColor", "foreground", "foregroundColor",
                 "checkState", "accessibleText", "accessibleDescription",
                 "sizeHint")
  createRoleList <- function()
    structure(vector("list", length(roleNames)), names = roleNames)
  roles <- createRoleList()
  cn <- sub("(^[^\\.].*)", ".\\1.display", colnames(df))
  getRoleNames <- function(x) gsub(".*\\.", "", x)
  dataRoles <- getRoleNames(cn)
  resolveRole <- function(role) {
    headerNames <- getHeaderNames(role)
    nheaders <- sapply(headerNames, length)
    headerNames[nheaders == 0L] <- list(header)
    headerNames <- unlist(headerNames)
    if (anyDuplicated(headerNames))
      stop("Redundant role information: ", paste(role, collapse=", "))
    role <- rep(role, nheaders)
    headerInd <- match(headerNames, header)
    map <- integer(length(header))
    map[headerInd] <- match(role, cn)
    map - 1L
  }
  resolvedRoles <- tapply(cn, factor(dataRoles, unique(dataRoles)), resolveRole,
                          simplify=FALSE)
  roles[names(resolvedRoles)] <- resolvedRoles
  attrs <- attributes(df)
  getHeaderRoles <- function(attrName, display = attrs[[attrName]]) {
    attrPrefix <- paste(attrName, ".", sep = "")
    headerAttrs <- grep(attrPrefix, names(attrs), fixed=TRUE, value=TRUE)
    headerRoles <- createRoleList()
    headerRoles$display <- display
    headerRoles[getRoleNames(headerAttrs)] <- attrs[headerAttrs]
    headerRoles
  }
  rowRoles <- getHeaderRoles("row.names")
  colRoles <- getHeaderRoles("names", header)
  .Call(qt_qsetDataFrame, model, df, roles, rowRoles, colRoles)
}

qdataFrame <- function(model) {
  stopifnot(inherits(model, "DataFrameModel"))
  .Call(qt_qdataFrame, model)
}
