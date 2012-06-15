echo EXPORTS > qtbase.def
nm -g --defined-only CMakeFiles/qtbase.dir/*.obj > tmp
sed -n '/^[^ ]* [T|C|B] /s/^[^ ]* [T|C|B] / /p' tmp >> qtbase.def
rm tmp

