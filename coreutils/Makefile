# (c) Itarge.Inc
all:
	cd coreutils-8.30; \
	make clean; \
	make distclean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install

install:
	cd coreutils-8.30; \
	make install

clean:
	cd coreutils-8.30; \
	make distclean
