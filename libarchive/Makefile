# (c) Itarge.Inc
all:
	cd libarchive-3.3.2; \
	make clean; \
	make distclean; \
	autoreconf -ivf; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--enable-shared \
	--enable-static \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	LIBS="-lm"; \
	make -j8 V=1; \
	make install

install:
	cd libarchive-3.3.2; \
	make install

clean:
	cd libarchive-3.3.2; \
	make distclean
