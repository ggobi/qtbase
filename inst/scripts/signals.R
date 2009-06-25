library(XML)
htmlErrorHandler <- function(msg, code, domain, line, col, level, filename) {
  if (level > 2)
    stop("Failed to Parse HTML [", line, ":", col, "]: ", msg)
}

setwd("~/research/src/qt-x11-opensource-src-4.5.0/doc/html/")
files <- dir(pattern="^q[^3]")

strs <- unlist(lapply(f, function(f) {
  dom <- htmlTreeParse(f, useInternalNodes=TRUE, error=htmlErrorHandler)
  path <- "/html//h3[text() = 'Signals']/following-sibling::ul[1]/li/text()"
  sapply(getNodeSet(dom, path), xmlValue)
}))

## wee bit of cleanup
strs <- gsub("[ )(,]", "", strs)
strs <- strs[!grepl("[=\302]", strs)]
strs <- sub("const", "const ", strs)

tab <- sort(table(strs))

types <- unique(strs)

getTypes <- function(x) {
  unlist(x)
