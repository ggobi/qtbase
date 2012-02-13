all:
	mkdir -p ../kdebindings-build;
	cd ../kdebindings-build; \
	cmake ../kdebindings -DCMAKE_INSTALL_PREFIX=../inst/local; \
	LD_LIBRARY_PATH=../../generator/bin make install
	mkdir -p ../src-build;
	cd ../src-build; \
	cmake ../src \
	      -DR_LIBRARIES="${LIBR}" \
	      -DR_INCLUDES=${R_INCLUDE_DIR} \
	      -DCMAKE_INSTALL_PREFIX=../src; \
	make install

clean:
	rm -rf ../kdebindings-build
	rm -rf ../src-build
	rm -rf ../inst/local
	rm -rf qtbase.so
