# (c) Itarge.Inc
all:
	cd libi2c; \
	make clean; \
	make CROSS=${COMPILE_PREFIX}; \
	cp -af src/libi2c.h ${PORTING_INSTALL_DIR}/include; \
	cp -af libi2c.a ${PORTING_INSTALL_DIR}/lib; \
	cp -af objs/i2c_test ${PLATFORM_SYSROOT_DIRECTORY}/usr/bin; \

install:
	cd libi2c; \
	cp -af src/libi2c.h ${PORTING_INSTALL_DIR}/include; \
	cp -af libi2c.a ${PORTING_INSTALL_DIR}/lib; \
	cp -af objs/i2c_test ${PLATFORM_SYSROOT_DIRECTORY}/usr/bin; \

clean:
	cd libi2c; \
	make clean
