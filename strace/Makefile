# (c) Itarge.Inc
all:
	cd strace-4.24; \
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
	--enable-mpers=check \
	--includedir=${PORTING_INSTALL_DIR}/include \
	--libdir=${PORTING_INSTALL_DIR}/libi; \
	make -j8 V=1; \
	make install

install:
	cd strace-4.24; \
	make install

clean:
	cd strace-4.24; \
	make distclean
