# (c) Itarge.Inc
all:
	cd libevent-2.1.8-stable; \
	make clean; \
	make distclean; \
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
	cp -af ${PORTING_INSTALL_DIR}/lib/libevent*.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

install:
	cd libevent-2.1.8-stable; \
	make install; \
	cp -af ${PORTING_INSTALL_DIR}/lib/libevent*.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

clean:
	cd libevent-2.1.8-stable; \
	make distclean
