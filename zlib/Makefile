# (c) Itarge.Inc
all:
	cd zlib-1.2.8; \
	make clean; \
	make distclean; \
	export CROSS_PREFIX=${COMPILE_PREFIX}; \
	./configure --prefix=${PORTING_INSTALL_DIR} --enable-shared; \
	make -j8 V=1; \
	make install

install:
	cd zlib-1.2.8; \
	make install

clean:
	cd zlib-1.2.8; \
	make distclean
