# (c) Itarge.Inc
all:
	cd bftpd-4.4; \
	make distclean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PLATFORM_SYSROOT_DIRECTORY}/usr \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make clean; \
	make CC=${PORTING_COMPILE_PREFIX}-gcc -j8 V=1; \
	make install; \
	chmod a+x ${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin/bftpd; \
	cp -af bftpd.conf ${PLATFORM_SYSROOT_DIRECTORY}/etc 

install:
	cd bftpd-4.4; \
	make install; \
	cp -af bftpd.conf ${PLATFORM_SYSROOT_DIRECTORY}/etc 

clean:
	cd bftpd-4.4; \
	make distclean
