## library(qtgui)

foo <- Qt$QPushButton()
## qsetDeleteOnClose(foo, TRUE)
foo

## user.data not yet supported (?)

bar <-
  qconnect(foo, "clicked", handler = function() {
      x <- foo
      if (x$text == "Yeah!")
          x$text <- "Done."
      else
          x$text <- "Yeah!"
  })

baz <-
  qconnect(foo, "clicked", function(x) {
             print(class(foo))
           }, foo)

##qdisconnect(foo, bar) # disconnect bar only

##qdisconnect(foo)


l <- Qt$QListWidget()
l$addItems(month.name)

handleActivation <- function(item)
{
    print(row)
}
qconnect(l, signal = "itemActivated",
         handler = handleActivation)    
l$show()

## connect to cell double-click in tables

ipkgs <- installed.packages()

library(qtmisc)
vpkgs <- qdataview(ipkgs)
qsetSortingEnabled(vpkgs, FALSE)
qresizeColumnsToContents(vpkgs)
vpkgs

qconnect(vpkgs, "cellDoubleClicked",
         function(x) {
           i <- qcurrentRow(x$vpkgs)
           j <- qcurrentColumn(x$vpkgs)
           print(x$ipkgs[i, j])
           old.pager <- getOption("pager")
           on.exit(options(pager = old.pager))
           options(pager = qpager)
           print(help(package = x$ipkgs[i, "Package"]))
         }, list(ipkgs = ipkgs, vpkgs = vpkgs))

## qdisconnect(vpkgs)

foo <- qeditor("/tmp/foo.R")

act <- qaction("Run R code", shortcut = "Ctrl+R", parent = foo)
qaddAction(foo, act)
qsetContextMenuPolicy(foo, "actions")

foo

bar <-
  qconnect(act, "triggered", 
           function(x) {
             print("action triggered")
           }, foo)
