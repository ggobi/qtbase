library(qtbase)


# Tests dynamic slots. This should not crash R.
# marshal_QString used to try to free the pointer to "x" in the anonymous function.
# However, the value of "x" was a member variable of QLineEdit ("a") and the
# deallocation failed. This was fixed by adding a flag to Smoke::StackItem
# indicating if the item was owned by the StackItem and should therefore be freed
# by marshal_QString or not.

a <- Qt$QLineEdit()
b <- Qt$QLineEdit()
qconnect(a, "textChanged", function(x) { b$text <- x })

b$text <- "1"
a$text <- "1"
if(!identical(a$text, b$text))
{
	stop("Encountered error in testing dynamic slots, ", a$text, " != ", b$text)
}
