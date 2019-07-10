# (c) Itarge.Inc
all:
	cd ncurses-6.0; \
	make clean; \
	make distclean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--with-shared \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install

install:
	cd ncurses-6.0; \
	make install

clean:
	cd ncurses-6.0; \
	make distclean
