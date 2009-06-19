
.noGenerics <- TRUE

.onUnload <- function(libpath)
{
    cat("Unloading qtbase\n")
    .Call(cleanupQtApp)
    library.dynam.unload("qtbase", libpath)
}

.onLoad <- function(libname, pkgname) 
{
    ## library.dynam("qtbase", pkgname, libname )
    .Call(addQtEventHandler)
}

setHook(packageEvent("qtbase", event = "detach"),
        function() {
            cat("Running detach hook for qtbase\n")
            .Call(cleanupQtApp)
        })


