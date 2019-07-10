# (c) Itarge.Inc
all:
	cd pixman-0.34.0; \
	make clean; \
	make distclean; \
	autoreconf -ivf; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--enable-shared \
	--enable-static \
	--disable-gtk \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	PKG_CONFIG_PATH=${PORTING_INSTALL_DIR}/lib/pkgconfig; \
	make -j8 V=1; \
	make install V=1

install:
	cd pixman-0.34.0; \
	make install

clean:
	cd pixman-0.34.0; \
	make distclean
