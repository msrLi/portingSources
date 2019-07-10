# (c) Itarge.Inc
all:
	cd libosip2-5.0.0; \
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
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install; \
	cp -af ${PORTING_INSTALL_DIR}/lib/libosip*.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

install:
	cd libosip2-5.0.0; \
	make install; \
	cp -af ${PORTING_INSTALL_DIR}/lib/libosip*.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

clean:
	cd libosip2-5.0.0; \
	make distclean
