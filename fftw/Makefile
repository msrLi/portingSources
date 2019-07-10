# (c) Itarge.Inc
all:
	cd fftw-3.3.8; \
	make clean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--enable-shared \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install

install:
	cd fftw-3.3.8; \
	make install

clean:
	cd fftw-3.3.8; \
	make clean
