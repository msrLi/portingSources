# (c) Itarge.Inc
all:
	cd xvidcore-1.3.5/build/generic; \
	make clean; \
	make distclean; \
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
	make -i install

install:
	cd xvidcore-1.3.5; \
	make install

clean:
	cd xvidcore-1.3.5; \
	make distclean
