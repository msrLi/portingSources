# (c) Itarge.Inc
all:
	cd libpng-1.6.26; \
	make clean; \
	make distclean; \
	./autogen.sh; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--enable-shared \
	--enable-static \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	PKG_CONFIG_PATH=${PORTING_INSTALL_DIR}/lib/pkgconfig; \
	make -j8 V=1; \
	make install; \
	cd ${PORTING_INSTALL_DIR}/include; \
	ln -sf libpng16 libpng

install:
	cd libpng-1.6.26; \
	make install; \
	cd ${PORTING_INSTALL_DIR}/include; \
	ln -sf libpng16 libpng

clean:
	cd libpng-1.6.26; \
	make distclean
