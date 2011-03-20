
.noGenerics <- TRUE

.onUnload <- function(libpath) {
  .Call(cleanupQtApp)
  library.dynam.unload("qtbase", libpath)
}

.onLoad <- function(libname, pkgname) 
{
  ## dir <- system.file(file.path("local", "lib"), package=pkgname)
  ## dyn.load(paste(file.path(dir, "libsmokeqt"), .Platform$dynlib.ext, sep=""))
  ## library.dynam("qtbase", pkgname, libname )

  qlibrary(Qt, NULL)

  ## Prefer OpenGL1.x engine, rather than the OpenGL2 ES engine
  ## R is usually running on non-mobile platforms
  ## This must be called before QApplication is constructed!
  if (!is.null(Qt$QGL$setPreferredPaintEngine))
    Qt$QGL$setPreferredPaintEngine(Qt$QPaintEngine$OpenGL)

  .Call(addQtEventHandler)
  reg.finalizer(getNamespace("qtbase"), function(ns)
                {
                  if ("qtbase" %in% loadedNamespaces())
                    .onUnload(file.path(libname, pkgname))
                }, onexit=TRUE)
}
