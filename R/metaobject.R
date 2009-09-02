qnormalizedSignature <- function(x) {
  .Call(qt_qnormalizedSignature, as.character(x))
}

qresolveSignature <- function(x, sig, type, nargs) {
  ## The user can specify a signal by name or by signature
  methods <- qmethods(x)
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

names.QObject <- function(x) {
    c(rownames(qproperties(x)), unique(qmethods(x)$name))
}

stripCall <- function(x) {
  gsub("\n *", "", sub("^[^:]* : *", "", x))
}

## Look for a property, then fall back to method. We could reverse
## this, but then we would have to ignore requests for accessors.
`$.QObject` <- function(x, name) {
  val <- try(qproperty(x, name), silent = TRUE)
  if (!inherits(val, "try-error"))
    val
  else {
    val2 <- try(NextMethod(), silent = TRUE)
    if (!inherits(val2, "try-error")) {
      msg <- stripCall(val)
      msg2 <- stripCall(val2)
      stop("Failed to select a method (", msg2,
           "), and failed to get a property (", msg, ")")
    } else val2
  }
}

## Just property setting
`$<-.QObject` <- function(x, name, value) {
  qproperty(x, name) <- value
  x
}

### Some stuff from QtRuby for creating a MetaData blob
##
## From the enum MethodFlags in qt-copy/src/tools/moc/generator.cpp
##
## AccessPrivate = 0x00
## AccessProtected = 0x01
## AccessPublic = 0x02
## MethodMethod = 0x00
## MethodSignal = 0x04
## MethodSlot = 0x08
## MethodCompatibility = 0x10
## MethodCloned = 0x20
## MethodScriptable = 0x40

## ## Keeps a hash of strings against their corresponding offsets
## ## within the qt_meta_stringdata sequence of null terminated
## ## strings. Returns a proc to get an offset given a string.
## ## That proc also adds new strings to the 'data' array, and updates 
## ## the corresponding 'pack_str' Array#pack template.
## def Internal.string_table_handler(data, pack_str)
## hsh = {}
## offset = 0
## return lambda do |str|
## if !hsh.has_key? str
## hsh[str] = offset
## data << str
## pack_str << "a*x"
## offset += str.length + 1
## end

## return hsh[str]
## end
## end

## def Internal.makeMetaData(classname, classinfos, dbus, signals, slots)
## ## Each entry in 'stringdata' corresponds to a string in the
## ## qt_meta_stringdata_<classname> structure.
## ## 'pack_string' is used to convert 'stringdata' into the
## ## binary sequence of null terminated strings for the metaObject
## stringdata = []
## pack_string = ""
## string_table = string_table_handler(stringdata, pack_string)

## ## This is used to create the array of uints that make up the
## ## qt_meta_data_<classname> structure in the metaObject
## data = [1, # revision
##         string_table.call(classname), 	# classname
##         classinfos.length, classinfos.length > 0 ? 10 : 0, 	# classinfo
##         signals.length + slots.length, 
##         10 + (2*classinfos.length), 	# methods
##         0, 0, # properties
##         0, 0] # enums/sets

## classinfos.each do |entry|
## data.push string_table.call(entry[0])		# key
## data.push string_table.call(entry[1])		# value
## end

## signals.each do |entry|
## data.push string_table.call(entry.full_name) # signature
## data.push string_table.call(entry.full_name.delete("^,"))	# parameters
## data.push string_table.call(entry.reply_type) # type, "" means void
## data.push string_table.call("")				# tag
## if dbus
## data.push MethodScriptable | MethodSignal | AccessPublic
## else
##   data.push MethodSignal | AccessProtected # flags, always protected for now
## end
## end

## slots.each do |entry|
## data.push string_table.call(entry.full_name) # signature
## data.push string_table.call(entry.full_name.delete("^,"))	# parameters
## data.push string_table.call(entry.reply_type) # type, "" means void
## data.push string_table.call("")				# tag
## if dbus
## data.push MethodScriptable | MethodSlot | AccessPublic
## else
##   data.push MethodSlot | AccessPublic # flags, always public for now
## end
## end

## data.push 0		# eod

## return [stringdata.pack(pack_string), data]
## end
