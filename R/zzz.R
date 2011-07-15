
.noGenerics <- TRUE

.onUnload <- function(libpath) {
  .Call("cleanupQtApp", PACKAGE="qtbase")
  library.dynam.unload("qtbase", libpath)
}

.onLoad <- function(libname, pkgname) 
{
  if (.Platform$OS.type=="windows") {
    lp <- gsub("/", "\\\\", paste(libname, pkgname, "bin", sep="/"))
    Sys.setenv(PATH = paste(lp, Sys.getenv("PATH"), sep=";"))
  }

  library.dynam("qtbase", pkgname, libname)

  qlibrary(Qt, NULL)

  ### HACK: populate QGlobalSpace now, as other Smoke libs will have
  ### the same class. Really, Smoke should namespace this thing, but
  ### maybe we cannot assume that every class has a unique name?
  Qt$QGlobalSpace
  
  ## Prefer OpenGL1.x engine, rather than the OpenGL2 ES engine
  ## R is usually running on non-mobile platforms
  ## This must be called before QApplication is constructed!
  if (!is.null(Qt$QGL$setPreferredPaintEngine))
    Qt$QGL$setPreferredPaintEngine(Qt$QPaintEngine$OpenGL)

  .Call("addQtEventHandler", PACKAGE="qtbase")
  reg.finalizer(getNamespace("qtbase"), function(ns)
                {
                  if ("qtbase" %in% loadedNamespaces())
                    .onUnload(file.path(libname, pkgname))
                }, onexit=TRUE)
}
