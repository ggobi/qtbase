
all:
	cmake ../inst/kdebindings/CMakeLists.txt \
	      -DCMAKE_INSTALL_PREFIX=../local
# Set LD_LIBRARY_PATH so that generator finds plugins -- workaround for bug
	LD_LIBRARY_PATH=../../generator/bin make -C ../inst/kdebindings install
	bash createProject.sh
	make -f Makefile.qtbase
	if test -e libqtbase.so; then cp libqtbase.so qtbase.so; fi
	## for macx:x
	if test -e libqtbase.dylib; then cp libqtbase.dylib qtbase.so; fi


clean:
	rm -f *.so *.o *.a libqt* qtbase.pro Makefile.qtbase moc*.cpp
	make -C ../inst/kdebindings clean
	rm -rf ../local


