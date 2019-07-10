# (c) Itarge.Inc
all:
	cd busybox-1.21.1; \
	make clean; \
	make distclean; \
	make defconfig; \
	sed -i -e 's,CONFIG_CROSS_COMPILER_PREFIX="",CONFIG_CROSS_COMPILER_PREFIX="${COMPILE_PREFIX}",g' .config; \
	sed -i -e 's,CONFIG_TELNETD=y,# CONFIG_TELNETD is not set,g' .config; \
	sed -i -e 's,CONFIG_TFTPD=y,# CONFIG_TFTPD is not set,g' .config; \
	sed -i -e 's,CONFIG_PREFIX="./_install",CONFIG_PREFIX="${PLATFORM_SYSROOT_DIRECTORY}",g' .config; \
	make -j8 V=1; \
	make install

install:
	cd busybox-1.21.1; \
	make install

clean:
	cd busybox-1.21.1; \
	make distclean
