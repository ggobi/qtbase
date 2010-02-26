
.noGenerics <- TRUE

.onUnload <- function(libpath)
{
    cat("Unloading qtbase\n")
    .Call(cleanupQtApp)
    library.dynam.unload("qtbase", libpath)
}

.onLoad <- function(libname, pkgname) 
{
  ## dir <- system.file(file.path("local", "lib"), package=pkgname)
  ## dyn.load(paste(file.path(dir, "libsmokeqt"), .Platform$dynlib.ext, sep=""))
  ## library.dynam("qtbase", pkgname, libname )
  .Call(addQtEventHandler)
  qlibrary(Qt, NULL)
}
