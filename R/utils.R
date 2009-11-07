
## Utility functions moved over from the formerly qtgui package

print.QWidget <- function(x, ...)
{
    x$show()
    invisible(x)
}



## functions below represent an attempted interface for faking REPL.
## It has not been used for a while and should eventually be removed.

.uEnv <- new.env(parent = emptyenv())

.u_assign <- function(value, name = "x")
{
    .uEnv[[name]] <- value
}

## FIXME: echo and print should default to user-settable options

.u_eval <- function(text, echo = TRUE, print = TRUE, env = .GlobalEnv)
{
    ## FIXME: echoing needs to get more sophisticated
    expr <- parse(text = text)
    exprSrc <- lapply(attr(expr, "srcref"), as.character)
    if (echo)
    {
        PS1 <- getOption("prompt")
        PS2 <- paste("\n", getOption("continue"), sep = "")
    }
    for (i in seq_along(expr))
    {
        if (echo) cat(paste(exprSrc[[i]], collapse = PS2), fill = TRUE)
        assign(".u_expr", expr[[i]], .GlobalEnv)
        ans <- evalq(withVisible(eval(.u_expr)), env)
        if (print && ans$visible) print(ans$value)
        if (echo) cat(PS1)
    }
    .uEnv[["last_value"]] <- ans
    invisible()
}

.u_tryEval <-
    function(text = .uEnv[["command"]],
             echo = TRUE, print = TRUE, env = .GlobalEnv)
{
    ans <- try(.u_eval(text, echo = echo, print = print, env = env),
               silent = TRUE)
    if (inherits(ans, "try-error"))
    {
        .uEnv[["last_value"]] <- ans
        if (echo) {
            cat(ans)
            cat(getOption("prompt"))
        }
    }
    invisible()
}


.u_lastValue <- function() .uEnv[["last_value"]]


    
