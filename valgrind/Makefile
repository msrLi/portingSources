# (c) Itarge.Inc
all:
	cd valgrind-3.14.0; \
	make clean; \
	make distclean; \
	autoreconf -ivf;\
	sed -i -e 's/armv7\*/armv7\* | arm/g' configure; \
	./configure --host=${PORTING_COMPILE_PREFIX} \
	--prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--sbindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/sbin \
	--enable-shared \
	--enable-static \
	CFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include" \
	CXXFLAGS="-O3 -I${PORTING_INSTALL_DIR}/include"; \
	make -j8 V=1; \
	make install

install:
	cd valgrind-3.14.0; \
	make install

clean:
	cd valgrind-3.14.0; \
	make distclean
