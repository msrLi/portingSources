# (c) Itarge.Inc
#
# -DENABLE_CJSON_TEST=On: Enable building the tests. (on by default)
# -DENABLE_CJSON_UTILS=On: Enable building cJSON_Utils. (off by default)
# -DENABLE_TARGET_EXPORT=On: Enable the export of CMake targets. Turn off if it makes problems. (on by default)
# -DENABLE_CUSTOM_COMPILER_FLAGS=On: Enable custom compiler flags (currently for Clang, GCC and MSVC). Turn off if it makes problems. (on by default)
# -DENABLE_VALGRIND=On: Run tests with valgrind. (off by default)
# -DENABLE_SANITIZERS=On: Compile cJSON with AddressSanitizer and UndefinedBehaviorSanitizer enabled (if possible). (off by default)
# -DENABLE_SAFE_STACK: Enable the SafeStack instrumentation pass. Currently only works with the Clang compiler. (off by default)
# -DBUILD_SHARED_LIBS=On: Build the shared libraries. (on by default)
# -DBUILD_SHARED_AND_STATIC_LIBS=On: Build both shared and static libraries. (off by default)
# -DCMAKE_INSTALL_PREFIX=/usr: Set a prefix for the installation.
# -DENABLE_LOCALES=On: Enable the usage of localeconv method. ( on by default )
# -DCJSON_OVERRIDE_BUILD_SHARED_LIBS=On: Enable overriding the value of BUILD_SHARED_LIBS with -DCJSON_BUILD_SHARED_LIBS.

#all:
#	cd cJSON; \
#	rm -rf build; \
#	mkdir -p build; \
#	cd build; \
#	echo "set (CMAKE_SYSTEM_NAME Linux)" >toolchain.cmake; \
#	echo "set (CMAKE_SYSTEM_VERSION 1)" >>toolchain.cmake; \
#	echo "set (CMAKE_SYSTEM_PROCESSOR "aarch64")" >>toolchain.cmake; \
#	echo "set (CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/bin/${PORTING_COMPILE_PREFIX}-g++")" >>toolchain.cmake; \
#	echo "set (CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/bin/${PORTING_COMPILE_PREFIX}-gcc")" >>toolchain.cmake; \
#	cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DENABLE_CJSON_UTILS=On -DENABLE_CJSON_TEST=On; \
#	make; \
#	make DESTDIR=${PORTING_INSTALL_DIR} install

all:
	cd cJSON; \
	make PREFIX=${PORTING_INSTALL_DIR} CC=${PORTING_COMPILE_PREFIX}-gcc; \
	make V=1; \
	make PREFIX=${PORTING_INSTALL_DIR} install; \
	cp -af ${PORTING_INSTALL_DIR}/lib/libcjson*.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

install:
	cd cJSON; \
	make PREFIX=${PORTING_INSTALL_DIR} install; \
	cp -af ${PORTING_INSTALL_DIR}/lib/libcjson*.so* ${PLATFORM_SYSROOT_DIRECTORY}/usr/lib

clean:
	cd cJSON; \
	make clean
