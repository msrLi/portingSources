# (c) Itarge.Inc
all:
	cd live.2019.03.06; \
	make clean; \
	cp -af config.armlinux config.${TARGET_CPU}; \
	sed -i -e 's;arm-elf-;${COMPILE_PREFIX};g' config.${TARGET_CPU}; \
	sed -i -e 's;=64;=64 -DNEWLOCALE_NOT_USED=1;g' config.${TARGET_CPU}; \
	./genMakefiles ${TARGET_CPU}; \
	make -j8; \
	make install PREFIX=${PORTING_INSTALL_DIR}; \
	${TOOLCHAIN_PATH}/bin/${COMPILE_PREFIX}strip testProgs/ite_auto_test_rtsp;	\
	cp testProgs/ite_auto_test_rtsp ${PLATFORM_SYSROOT_DIRECTORY}/usr/bin;	\
	rm -rf config.${TARGET_CPU}

install:
	cd live.2019.03.06; \
	make install PREFIX=${PORTING_INSTALL_DIR}	\
	${TOOLCHAIN_PATH}/bin/${COMPILE_PREFIX}strip testProgs/ite_auto_test_rtsp;	\
	cp testProgs/ite_auto_test_rtsp ${PLATFORM_SYSROOT_DIRECTORY}/usr/bin

clean:
	cd live.2019.03.06; \
	make clean
