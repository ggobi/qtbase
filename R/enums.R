## convenience functions for bitwise operations on enums/flags

## A QtEnum is a enum value represented by an R integer vector.

## QFlags arise through '|' combination of enum values. Smoke
## communicates QFlags as plain unsigned integers. Thus, we do not
## know when we have a QFlags. As a general policy, we convert all
## unsigned integers to double R vectors, so as not to cause confusion
## when flowing into the sign bit.

## In the below, we coerce the result to a double, so that we can
## disambiguate method calls (methods often accept either an enum or
## flag of the same type) based solely on the type of the vector
## (integer for enums, double for flags).

"|.QtEnum" <- function(x, y) {
  as.numeric(packBits(intToBits(x) | intToBits(y), "integer"))
}

"&.QtEnum" <- function(x, y) {
  as.numeric(packBits(intToBits(x) & intToBits(y), "integer"))
}
