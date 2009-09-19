
all:
	cmake ../inst/kdebindings/CMakeLists.txt \
	      -DCMAKE_INSTALL_PREFIX=../local
	make -C ../inst/kdebindings install
	cp ../local/lib/* . # to be placed into 'libs'
	bash createProject.sh
	make -f Makefile.qtbase
	if test -e libqtbase.so; then cp libqtbase.so qtbase.so; fi
	## for macx:
	if test -e libqtbase.dylib; then cp libqtbase.dylib qtbase.so; fi


clean:
	rm -f *.so *.o *.a libqt* qtbase.pro Makefile.qtbase moc*.cpp
	make -C ../inst/kdebindings clean
	rm -rf ../local


