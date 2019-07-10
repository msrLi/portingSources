# (c) Itarge.Inc
all:
	pushd buildroot-2019.02/; \
	rm -rf output; \
	make clean; \
	make ${TARGET_CPU}_defconfig; \
	sed -i -e "s;PLATFORM_ROOT_PATH;${PLATFORM_TOP_PATH};g" .config; \
	make; \
	popd

install:
	pushd buildroot-2019.02/; \
	popd

clean:
	rm -rf buildroot-2019.02/output
