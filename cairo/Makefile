# (c) Itarge.Inc
all:
	cd cairo-1.14.8; \
	make clean; \
	make distclean; \
	./autogen.sh; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--enable-shared \
	--enable-static \
	--enable-xlib=no \
	--enable-xcb=no \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	PKG_CONFIG_PATH=${PORTING_INSTALL_DIR}/lib/pkgconfig; \
	make -j8 V=1; \
	make install V=1

install:
	cd cairo-1.14.8; \
	make install

clean:
	cd cairo-1.14.8; \
	make distclean
