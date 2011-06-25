## methods on RQtObject, which wraps every object created by Smoke

setOldClass("RQtObject")

print.RQtObject <- function(x, ...) {
  cat(head(class(x), 1), "instance\n")
}
