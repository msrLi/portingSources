# (c) Itarge.Inc
all:
	cd nmealib-0.5.3; \
	make clean; \
	make COMPILE_PREFIX=${COMPILE_PREFIX} V=1; \
	cp -af include/nmea ${PORTING_INSTALL_DIR}/include; \
	cp -af lib/libnmea.a ${PORTING_INSTALL_DIR}/lib

install:
	cd nmealib-0.5.3; \
	cp -af include/nmea ${PORTING_INSTALL_DIR}/include; \
	cp -af lib/libnmea.a ${PORTING_INSTALL_DIR}/lib

clean:
	cd nmealib-0.5.3; \
	make clean
