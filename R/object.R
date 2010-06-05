## methods on RQtObject, which wraps every object created by Smoke


print.RQtObject <- function(x, ...) {
  cat(head(class(x), 1), "instance\n")
}
