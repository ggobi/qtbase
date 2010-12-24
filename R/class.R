### Classes are functions (constructors) with static methods in an environment

## invoke a static method
"$.RQtClass" <- function(x, name) {
  attr(x, "env")[[name]]
}
"[[.RQtClass" <- function(x, name) {
  attr(x, "env")[[name]]
}


names.RQtClass <- function(x) {
  ls(attr(x, "env"))
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

print.RQtClass <- function(x, ...) {
  methods <- subset(qmethods(x), !protected)
  cat("Class '", attr(x, "name"), "' with ", nrow(methods), " public methods\n",
      sep = "")
}

## Smoke classes are populated with their entire hierarchy, as
## environment inheritance is single, while C++ inheritance is
## multiple.

## obtain a class object from a smoke module and a name
qsmokeClass <- function(x, name, internals = character()) {
  env <- new.env(parent = emptyenv())
  basename <- gsub(".*::", "", name)
  cl <- structure(function(...) qinvokeStatic(cl, basename, ...), name = name,
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
    assign(gsub(".*::", "", internal), qsmokeClass(x, internal), env)
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

qsetClass <- function(name, parent, constructor = function(...) parent(...),
                      where = topenv(parent.frame()))
{
  ## mangle the class name to prevent conflicts
  module <- getPackageName(where)
  prefixedName <- paste("R", module, name, sep = "::")
  if (exists(name, where))
    warning("Symbol '", name, "' already exists in '", module, "'")
  ## get our real constructor
  constructor <- normConstructor(constructor, parent)
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
  metadata <- new.env(parent = emptyenv())
  metadata$properties <- new.env(parent = emptyenv())
  cl <- structure(constructor, module = module, name = prefixedName,
                  parent = parent, env = env, instanceEnv = instanceEnv,
                  metadata = metadata,
                  class = c("RQtUserClass", "RQtClass", "function"))
  assign(name, cl, where)
  cl
}

qcast <- function(x, class) {
  .Call(qt_qcast, x, class)
}

qenclose <- function(x, fun) {
  .Call(qt_qenclose, x, fun)
}

qsetMethod <- function(name, class, FUN,
                       access = c("public", "protected", "private"))
{
  attr(FUN, "access") <- match.arg(access)
  assign(name, FUN, attr(class, "instanceEnv"))
  name
}

## Integration with the Qt Meta Object Compiler (MOC)

## The basic idea: define methods in R that are described by
## QMetaObject. This allows R to define signals and slots (and
## properties, enums, etc). The utility of signals is obvious. Slots
## could be exposed as e.g. DBus services. The main downside is that
## providing an external interface requires us to specify the types
## using C++ nomenclature.

## The methods will belong to the R class, as usual. We will compile a
## QMetaObject and provide it via the QObject::metaObject() virtual
## method. Then we will catch invocations via the QObject::qt_metacall
## virtual. All methods could be forwarded to R, but we might
## short-circuit signal emissions (and call QMetaObject::activate).

qsetSlot <- function(signature, class, FUN,
                     access = c("public", "protected", "private"))
{
  access <- match.arg(access)
  method <- qmetaMethod(signature, access, names(formals(FUN)))
  qsetMethod(method$name, class, FUN, access)
  qmetadata(class)$slots[[signature]] <- method
  method$name
}

## Signals are essentially implemented by QMetaObject::activate(). We
## could have the signal method call this directly, but for
## convenience we instead call the corresponding QMetaMethod. This is
## caught by the qt_metacall override which then calls
## QMetaObject::activate().

qsetSignal <- function(signature, class,
                       access = c("public", "protected", "private"))
{
  access <- match.arg(access)
  method <- qmetaMethod(signature, access)
  qmetadata(class)$signals[[signature]] <- method
  meta <- qmetaObject(class)
  index <- meta$methodCount() - 1L
  qsetMethod(method$name, class,
             function(...) .Call(qt_qmetaInvoke, this, index, list(...)),
             access)
  method$name
}

qsetProperty <- function(name, class, type,
                         read = function() this[[.name]],
                         write = function(val) this[[.name]] <- val,
                         ##reset = NULL,
                         notify = NULL, 
                         constant = FALSE, final = FALSE,
                         ##designable = TRUE, scriptable = TRUE,
                         stored = TRUE, user = FALSE)
{
  if (missing(name))
    stop("'name' is required")
  if (missing(class))
    stop("'class' is required")
  if (missing(type))
    stop("'type' is required")
  .name <- paste(".", name, sep = "")
  prop <- list(name = name, type = type, read = read,
               write = write, notify = notify, constant = constant,
               final = final, stored = stored, user = user)
  qmetadata(class)$properties[[name]] <- prop
  name
}
