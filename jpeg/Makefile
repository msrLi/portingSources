# (c) Itarge.Inc
all:
	cd jpeg-9a; \
	make clean; \
	make distclean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--disable-shared \
	--enable-static \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install; \
	mv ${PORTING_INSTALL_DIR}/lib/libjpeg.a ${PORTING_INSTALL_DIR}/lib/libitejpeg.a; \
	mv ${PORTING_INSTALL_DIR}/lib/libjpeg.la ${PORTING_INSTALL_DIR}/lib/libitejpeg.la

install:
	cd jpeg-9a; \
	make install; \
	mv ${PORTING_INSTALL_DIR}/lib/libjpeg.a ${PORTING_INSTALL_DIR}/lib/libitejpeg.a; \
	mv ${PORTING_INSTALL_DIR}/lib/libjpeg.la ${PORTING_INSTALL_DIR}/lib/libitejpeg.la

clean:
	cd jpeg-9a; \
	make distclean
