# (c) Itarge.Inc
all:
	cd gdb-7.8.2; \
	make clean; \
	make distclean; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--enable-shared \
	--enable-static \
	--without-x \
	--disable-gdbtk \
	--disable-tui \
	--without-included-regex \
	--without-included-gettext \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install; \
	${PORTING_COMPILE_PREFIX}-strip ${PLATFORM_SYSROOT_DIRECTORY}/usr/bin/gdb

install:
	cd gdb-7.8.2; \
	make install; \
	${PORTING_COMPILE_PREFIX}-strip ${PLATFORM_SYSROOT_DIRECTORY}/usr/bin/gdb

clean:
	cd gdb-7.8.2; \
	make distclean
