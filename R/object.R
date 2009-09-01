## methods on SmokeObject, which wraps every object created by Smoke

### FIXME: really should check here that the method name exists
"$.SmokeObject" <- function(x, name) {
  function(...) qinvoke(x, name, ...)
}
