# (c) Itarge.Inc

ifeq (${ARCH}, arm)
ARCH_MATCH=arm
else
ARCH_MATCH=aarch64
endif

all:
	cd ffmpeg-3.4.2; \
	make clean; \
	make distclean; \
	./configure --prefix=${PORTING_INSTALL_DIR} \
	--bindir=${PLATFORM_SYSROOT_DIRECTORY}/usr/bin \
	--enable-shared \
	--enable-static \
	--enable-gpl \
	--cross-prefix=${COMPILE_PREFIX} \
	--enable-cross-compile \
	--arch=${ARCH_MATCH} \
	--disable-stripping \
	--disable-linux-perf \
	--target-os=linux \
	--enable-libx264 \
	--enable-libxvid \
	--cc=${PORTING_COMPILE_PREFIX}-gcc \
	--enable-swscale \
	--enable-neon \
	--extra-cflags="-O3 -I${PORTING_INSTALL_DIR}/include" \
	--extra-ldflags="-L${PORTING_INSTALL_DIR}/lib" \
	--pkgconfigdir="${PORTING_INSTALL_DIR}/lib/pkgconfig"; \
	make -j8; \
	make install

install:
	cd ffmpeg-3.4.2; \
	make install

clean:
	cd ffmpeg-3.4.2; \
	make distclean
