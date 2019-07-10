# (c) Itarge.Inc
all:
	cd upgradeserver; \
	make clean; \
	make distclean; \
	make -j8 V=1; \
	cp -af include/* ${PORTING_INSTALL_DIR}/include; \
	cp -af ${ITARGE_BUILD_OBJS_PATH}/upgradeserver/libs/* ${PORTING_INSTALL_DIR}/lib

install:
	cd upgradeserver; \
	cp -af include/* ${PORTING_INSTALL_DIR}/include; \
	cp -af ${ITARGE_BUILD_OBJS_PATH}/upgradeserver/libs/* ${PORTING_INSTALL_DIR}/lib

clean:
	cd upgradeserver; \
	make distclean
