# (c) Itarge.Inc
all:
	cd openssl-1.0.2k; \
	make clean; \
	AR=${COMPILE_PREFIX}ar RANLIB=${COMPILE_PREFIX}ranlib  CC=${COMPILE_PREFIX}gcc \
	./Configure --prefix=${PORTING_INSTALL_DIR} no-asm shared linux-armv4; \
	make -j8 V=1; \
	make install

install:
	cd openssl-1.0.2k; \
	make install

clean:
	cd openssl-1.0.2k; \
	make clean
