# (c) Itarge.Inc
export ACE_ROOT := ${PWD}/ACE/ACE_wrappers
export INSTALL_PREFIX := ${PORTING_INSTALL_DIR}

all: 
	cd ACE_wrappers; \
	cp -af ${ACE_ROOT}/ace/config-linux.h ${ACE_ROOT}/ace/config.h; \
	cp -af ${ACE_ROOT}/include/makeinclude/platform_linux.GNU ${ACE_ROOT}/include/makeinclude/platform_macros.GNU; \
	vim -c "9,11 d" -c "wq!" -c "q!" ${ACE_ROOT}/include/makeinclude/platform_macros.GNU; \
	vim -c "5,5 d" -c "wq!" -c "q!" ${ACE_ROOT}/include/makeinclude/platform_macros.GNU; \
	sed -i -e 's/?= gcc/= '${COMPILE_PREFIX}'gcc/g' ${ACE_ROOT}/include/makeinclude/platform_macros.GNU; \
	sed -i -e 's/?= g++/= '${COMPILE_PREFIX}'g++/g' ${ACE_ROOT}/include/makeinclude/platform_macros.GNU; \
	make clean; \
	make -j8 V=1; \
	make static_libs=1 -j8 V=1; \
	make install

install:
	cd ACE_wrappers; \
	make install

clean:
	cd ACE_wrappers; \
	make clean
