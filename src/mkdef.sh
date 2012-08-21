echo EXPORTS > qtbase.def
nm -g --defined-only CMakeFiles/qtbase.dir/*.obj > tmp
sed -n $1 tmp >> qtbase.def
rm tmp

