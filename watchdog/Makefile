# (c) Itarge.Inc
all:
	cd watchdog-5.15; \
	make clean; \
	make distclean; \
	autoreconf -ivf;\
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--sysconfdir=${PLATFORM_SYSROOT_DIRECTORY}/etc \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make DESTDIR=${PLATFORM_SYSROOT_DIRECTORY} install

install:
	cd watchdog-5.15; \
	make DESTDIR=${PLATFORM_SYSROOT_DIRECTORY} install

clean:
	cd watchdog-5.15; \
	make distclean
