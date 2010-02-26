### Classes are functions (constructors) with static methods in an environment

## invoke a static method
"$.RQtClass" <- function(x, name) {
  attr(x, "env")[[name]]
}

qmethods <- function(x) {
  stopifnot(is(x, "RQtClass"))
  methods <- .Call(qt_qmethods, x)
  names(methods) <- c("name", "return", "signature", "protected", "static")
  df <- as.data.frame(methods, stringsAsFactors=FALSE)
  df[!duplicated(df$signature),]
}

qenums <- function(x) {
  stopifnot(is(x, "RQtClass"))
  .Call(qt_qenums, x)
}

print.RQtClass <- function(x) {
  methods <- subset(qmethods(x), !protected)
  cat("Class '", attr(x, "name"), "' with ", nrow(methods), " public methods\n",
      sep = "")
}

## Smoke classes are populated with their entire hierarchy, as
## environment inheritance is single, while C++ inheritance is
## multiple.

## obtain a class object from a smoke module and a name
qsmokeClass <- function(x, name) {
  env <- new.env(parent = emptyenv())
  cl <- structure(function(...) qinvokeStatic(cl, name, ...), name = name,
                  env = env, module = attr(x, "name"),
                  class = c("RQtSmokeClass", "RQtClass", "function"))
  methods <- qmethods(cl)
  methods <- subset(methods, !duplicated(name) & static & !protected)
  lapply(methods$name, function(name) {
    fun <- structure(function(...) qinvokeStatic(cl, name, ...), static = TRUE)
    assign(name, fun, env)
  })
  enums <- qenums(cl)
  for (enum in names(enums))
    assign(enum, structure(enums[enum], class = "QtEnum"), env)
  internals <- grep(paste("^", name, "::", sep = ""), qclasses(x), value = TRUE)
  for (internal in internals)
    assign(internal, qsmokeClass(x, internal), env)
  lockEnvironment(env, TRUE)
  cl
}

## Rewrites the constructor to handle base initialization
normConstructor <- function(x, parent) {
  if (!is.function(x))
    stop("constructor must be a function")

  ## Find base constructor call (the first one)
  b <- body(x)
  first <- which(sapply(b, identical, as.name("{"))) + 1
  expr <- NULL
  if (!length(first)) # no braces
    expr <- b
  else if (first <= length(b))
    expr <- b[[first]]
  if (is.call(expr) && identical(expr[[1]], as.name("super"))) {
    baseCall <- expr
    baseCall[[1]] <- parent
    if (length(first))
      body(x)[[first]] <- NULL
  } else baseCall <- as.call(list(parent)) # make default
  
  ## Stick the call into its own function, so we can enclose it
  baseConstructor <- as.function(c(formals(x), baseCall))
  environment(baseConstructor) <- environment(x)
  
  ## Now generate a new function that:
  ## - executes the base constructor
  ## - casts the base instance down to this class
  ## - encloses the constructor in the instance env (enclosed by original env)
  ## - invokes the constructor
  wrapperBody <- substitute({
    mc <- as.list(match.call())[-1]
    base <- do.call(baseConstructor, mc)
    this <- qcast(base, sys.function())
    do.call(qenclose(this, x), mc)
    this
  }, list(x = x, baseConstructor = baseConstructor))
  if (!length(first)) # no need to call constructor
    wrapperBody[[5]] <- NULL
  fun <- as.function(c(formals(x), wrapperBody))
  environment(fun) <- getNamespace("qtbase")
  fun
}

qsetClass <- function(x, parent, constructor = function(...) parent(...),
                      where = topenv(parent.frame()))
{
  ## get our real constructor
  constructor <- normConstructor(constructor, parent)
  ## mangle the class name to prevent conflicts
  module <- getPackageName(where)
  name <- paste("R", module, x, sep = "::")
  ### FIXME: May want to support reregistration of classes. This requires:
  ### 1) chaining up at the C++ Class level, rather than at instanceEnv
  ### 2) reducing the 'parent' attribute to a light-weight reference
  ### 3) add ability to unregister classes from cache
  ### NOTE: Not sure if this is a good idea, since it breaks instances
  parentEnv <- attr(parent, "instanceEnv")
  if (is.null(parentEnv))
    parentEnv <- emptyenv() # a smoke class, no instance symbols
  instanceEnv <- new.env(parent = parentEnv)
  env <- attr(parent, "env") # do not support user static methods yet
  cl <- structure(constructor, module = module, name = name, parent = parent,
                  env = env, instanceEnv = instanceEnv,
                  class = c("RQtUserClass", "RQtClass", "function"))
  assign(x, cl, where)
}

qcast <- function(x, class) {
  .Call(qt_qcast, x, class)
}

qenclose <- function(x, fun) {
  .Call(qt_qenclose, x, fun)
}

qsetMethod <- function(name, class, FUN,
                       access = c("public", "protected", "private"),
                       slot = FALSE) # 'slot' not supported yet
{
  attr(FUN, "access") <- match.arg(access)
  assign(name, FUN, attr(class, "instanceEnv"))
  if (isTRUE(slot))
    slot <- ""
  if (is.character(slot)) {
    for (s in slot)
      qsetSlot(name, class, s)
  }
}

qsetSlot <- function(name, class, params)
{
  sig <- paste(name, "(", params, ")", sep = "")
  fun <- get(name, attr(class, "instanceEnv"))
  qmetadata(x)$slots[[sig]] <-
    list(signature = sig, args = names(formals(fun)),
         access = attr(fun, "access"))
}

