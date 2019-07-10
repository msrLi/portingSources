# (c) Itarge.Inc
all:
	cd ImageMagick-7.0.5-4; \
	make clean; \
	make distclean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--enable-shared \
	--enable-static \
	--disable-openmp \
	--without-perl \
	--without-x \
	--without-openexr \
	--without-pango \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	PKG_CONFIG_PATH=${PORTING_INSTALL_DIR}/lib/pkgconfig; \
	make -j8 V=1; \
	make install

install:
	cd ImageMagick-7.0.5-4; \
	make install

clean:
	cd ImageMagick-7.0.5-4; \
	make distclean
