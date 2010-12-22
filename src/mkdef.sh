echo EXPORTS > qtbase.def
nm -g --defined-only CMakeFiles/qtbase.dir/*.obj > tmp
sed -n '/^........ [T|C|B] _/s/^........ [T|C|B] _/ /p' tmp >> qtbase.def
rm tmp

