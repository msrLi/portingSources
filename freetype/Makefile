# (c) Itarge.Inc
all:
	cd freetype-2.7; \
	make clean; \
	make distclean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--enable-shared \
	--enable-static \
	--with-png=no \
	--with-harfbuzz=no \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install

install:
	cd freetype-2.7; \
	make install

clean:
	cd freetype-2.7; \
	make distclean
