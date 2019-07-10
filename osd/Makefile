# (c) Itarge.Inc
all:
	cd swosd; \
	make clean; \
	make distclean; \
	make -j8 V=1; \
	cp -af include/* ${PORTING_INSTALL_DIR}/include; \
	cp -af ${ITARGE_BUILD_OBJS_PATH}/swosd/libs/* ${PORTING_INSTALL_DIR}/lib

install:
	cd swosd; \
	cp -af include/* ${PORTING_INSTALL_DIR}/include; \
	cp -af ${ITARGE_BUILD_OBJS_PATH}/swosd/libs/* ${PORTING_INSTALL_DIR}/lib

clean:
	cd swosd; \
	make distclean
