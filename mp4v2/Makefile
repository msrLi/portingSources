# (c) Itarge.Inc
all:
	cd mp4v2-2.0.0; \
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
	cp -af ${PORTING_INSTALL_DIR}/lib/libmp4v2.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

install:
	cd mp4v2-2.0.0; \
	make install; \
	cp -af ${PORTING_INSTALL_DIR}/lib/libmp4v2.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

clean:
	cd mp4v2-2.0.0; \
	make distclean
