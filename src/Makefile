
all:
	./createProject.sh
	make -f Makefile.qtbase
	if test -e libqtbase.so; then cp libqtbase.so qtbase.so; fi
	## for macx:
	if test -e libqtbase.dylib; then cp libqtbase.dylib qtbase.so; fi


clean:
	rm -f *.so *.o *.a libqt* qtbase.pro Makefile.qtbase moc*.cpp


