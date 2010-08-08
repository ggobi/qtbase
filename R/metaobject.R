qmocMethods <- function(x) {
  stopifnot(is(x, "QObject"))
  methods <- .Call(qt_qmocMethods, x)
  methods[[1]] <- c("method", "signal", "slot", "constructor")[methods[[1]] + 1]
  methods <- c(list(sub("\\(.*", "", methods[[2]])), methods)
  names(methods) <- c("name", "type", "signature", "return", "nargs")
  as.data.frame(methods, stringsAsFactors=FALSE, row.names = methods$signature)
}

qnormalizedSignature <- function(x) {
  .Call(qt_qnormalizedSignature, as.character(x))
}

qresolveSignature <- function(x, sig, type, nargs) {
  ## The user can specify a signal by name or by signature
  methods <- qmocMethods(x)
  if (!missing(type))
    methods <- subset(methods, type == type)
  sigs <- as.character(methods$signature)
  if (!length(grep("\\(", sig))) {
    sigs <- sigs[grep(paste("^", sig, "\\(", sep = ""), sigs)]
    if (length(sigs) > 1) {
      argmatch <- (if (missing(nargs)) 0 else nargs) == methods[sigs, "nargs"]
      if (sum(argmatch) == 0) # just take first, will fail below
        argmatch[1] <- TRUE
      if (sum(argmatch) > 1)
        stop("ambiguous method selection: ", paste(sigs, collapse=", "))
      sigs <- sigs[argmatch]
    }
    if (!length(sigs))
      stop("method does not exist")
    sig <- sigs[1]
  } else {
    sig <- qnormalizedSignature(sig)
    if (!(sig %in% sigs))
      stop("method does not exist")
  }
  ## slots cannot have default arguments, usually signals are multiplied
  if (!missing(nargs) && methods[sig, "nargs"] != nargs)
    stop("number of arguments does not match method signature")
  sig
}

qproperties <- function(x) {
  stopifnot(is(x, "QObject"))
  props <- .Call(qt_qproperties, x)
  names(props) <- c("name", "type", "readable", "writable")
  name <- props$name
  props$name <- NULL
  as.data.frame(props, row.names=name)
}

qmetadata <- function(x) {
  attr(x, "metadata")
}

## Every time metadata is set, we recompile it and reset the methods
## so that they refer to the new metadata
"qmetadata<-" <- function(x, value) {
  attr(x, "metadata") <- value
  compiled <- compileMetaObject(x)
  attr(x, "metaObject") <- compiled
  qsetMethod("metaObject", x, function() compiled)
  ## Lets MocClass see our methods
  qsetMethod("staticMetaObject", x, function() compiled)
  ## Ensure this method is defined
  qsetMethod("qt_metacall", x,
             function(call, id, args) .Call(qt_qmetacall, this, call, id, args))
  x
}

qmetaObject <- function(x) {
  attr(x, "metaObject")
}

### Some stuff derived from QtRuby for creating a MetaData blob

## 'x' is a class
## 'metadata' is an environment of classinfos, signals and slots

### NOTE: This does not work yet.
### It would let an R class define:
### - Slots: useful for providing dbus services, and easy
### - Signals: very useful
### - Properties: Encapsulated fields are nice, but do we want to be bound
###   by QValue? Could just implement our own custom properties.
### - Class info: useful for describing dbus services
### X Enums: probably not necessary

compileMetaObject <- function(x) {
  metadata <- qmetadata(x)
  
  infos <- metadata[["classinfos"]]
  signals <- metadata[["signals"]]
  slots <- metadata[["slots"]]

  ## generate 'stringdata' table
  allNames <- unique(c(unlist(metadata), attr(x, "name"), ""))
  offsets <- cumsum(c(0, head(nchar(allNames), -1) + 1))
  names(offsets) <- allNames
  
  stringdata <- charToRaw(paste(allNames, collapse=";"))
  stringdata[stringdata == charToRaw(";")] <- as.raw(0)

  ## generate 'data' table

  ##
  ## From the enum MethodFlags in qt-copy/src/tools/moc/generator.cpp
  ##
  AccessPrivate <- 0x00
  AccessProtected <- 0x01
  AccessPublic <- 0x02
  MethodMethod <- 0x00
  MethodSignal <- 0x04
  MethodSlot <- 0x08
  MethodCompatibility <- 0x10
  MethodCloned <- 0x20
  MethodScriptable <- 0x40

  access <- c(private = AccessPrivate, protected = AccessProtected,
              public = AccessPublic)
  
  data <-
    c(1, # revision
      offsets[attr(x, "name")], 	# classname
      length(infos), ifelse(length(infos) > 0, 10, 0), # class info
      length(signals) + length(slots), 10 + (2*length(infos)), # methods
      0, 0, # properties
      0, 0) # enums

  ## the class info
  data <- c(data, offsets[rbind(names(infos), infos)])

  methodData <- function(methods, flag) {
    do.call("c", lapply(methods, function(method) {
      ## cannot use character extraction due to empty strings
      offm <- match(c(method$signature, method$args, method$type, ""),
                    names(offsets))
      c(offsets[offm], access[method$access] + flag + MethodScriptable)
    }))
  }
  
  ## the signals
  data <- c(data, methodData(signals, MethodSignal))
  
  ## the slots
  data <- c(data, methodData(slots, MethodSlot))

  .Call(qt_qnewMetaObject, x, stringdata, data)
}
