library(qtgui)

foo <- qpushButton()
qsetDeleteOnClose(foo, TRUE)
foo

bar <-
  qconnect(foo, "clicked", handler = function(x) {
    if (qtext(x) == "Yeah!")
      qsetText(x, "Done.")
    else
      qsetText(x, "Yeah!")
  }, foo)

baz <-
  qconnect(foo, "clicked", function(x) {
             print(class(x))
             print(qtext(x))
           }, foo)


zab <-
  qconnect(foo, "clicked", function(x) {
             print("destroyed")
           }, NULL)



qdisconnect(foo, bar) # disconnect bar only

qdisconnect(foo)

qconnect(foo, "clicked", function(x) {
           print(class(x))
           print(qtext(x))
         }, foo)

qdisconnect(foo)

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
