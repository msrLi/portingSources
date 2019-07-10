# (c) Itarge.Inc
# cmake ../ -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON 
all:
	rm -rf opencv-3.4.2/arm; \
	mkdir -p opencv-3.4.2/arm; \
	cd opencv-3.4.2/arm; \
	cmake "Unix Makefiles" .. \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_C_COMPILER=${COMPILE_PREFIX}gcc \
	-DCMAKE_CXX_COMPILER=${COMPILE_PREFIX}g++  \
	-DCMAKE_SYSTEM_PROCESSOR=${PORTING_COMPILE_PREFIX} \
	-DTOOLCHAIN_DIR=${TOOLCHAIN_PATH} \
	-DCMAKE_FIND_ROOT_PATH=${TOOLCHAIN_PATH} \
	-DARM_LINUX_SYSROOT=${TOOLCHAIN_PATH} \
	-DWITH_IPP=OFF  \
	-DWITH_TBB=OFF  \
	-DWITH_ITT=OFF  \
	-DENABLE_NEON=ON \
	-DCMAKE_C_FLAGS="-I${PORTING_INSTALL_DIR}/include" \
	-DCMAKE_CXX_FLAGS="-I${PORTING_INSTALL_DIR}/include " \
	-DCMAKE_EXE_LINKER_FLAGS="-Wl,-rpath-link=${PORTING_INSTALL_DIR}/lib" \
	-DCMAKE_INSTALL_PREFIX=${PORTING_INSTALL_DIR};  \
	make -j20; \
	make install

install:
	cd opencv-3.4.2/arm; \
	make install

clean:
	rm -rf opencv-3.4.2/arm
