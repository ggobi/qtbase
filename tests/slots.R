library(qtbase)

a <- Qt$QLineEdit()
b <- Qt$QLineEdit()
qconnect(a, "textChanged", function(x) { b$text <- x })

b$text <- "1"
a$text <- "1"
if(!identical(a$text, b$text))
{
	stop("Encountered error in testing dynamic slots, ", a$text, " != ", b$text)
}
