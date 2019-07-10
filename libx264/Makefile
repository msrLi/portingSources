# (c) Itarge.Inc
all:
	cd x264-snapshot-20170521-2245; \
	make clean; \
	make distclean; \
	CC=${PORTING_COMPILE_PREFIX}-gcc \
	STRIP=${PORTING_COMPILE_PREFIX}-strip \
	LD=${PORTING_COMPILE_PREFIX}-ld \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--enable-shared \
	--enable-static \
	--extra-cflags="-O3 -I${PORTING_INSTALL_DIR}/include" \
	--extra-ldflags="-L${PORTING_INSTALL_DIR}/lib"; \
	make -j8 V=1; \
	make install

install:
	cd x264-snapshot-20170521-2245; \
	make install

clean:
	cd x264-snapshot-20170521-2245; \
	make distclean
