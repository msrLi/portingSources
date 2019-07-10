THIRD_PARTY_DIRS = zlib xz lzo jpeg openssl libi2c nmealib libuuid mtd-utils readline freetype ncurses \
                   libxml2 osip2 eXosip libevent expat ACE bftpd libarchive cJSON iperf libpcap tcpdump \
                   libiconv valgrind strace mp4v2 live555

ITE_PRIVATE_DIRS = osd upgrade-server

ifeq (${ITE_OPENCV_ENABLE}, 1)
ITE_OPENCY_DIRS = libpng yasm libx264 libxvid ffmpeg opencv
else
ITE_OPENCY_DIRS =
endif

ifeq (${ITE_ALG_DEPENDS_ENABLE}, 1)
ITE_ALG_DEPENDS_DIRS = fftw
else
ITE_ALG_DEPENDS_DIRS =
endif

ifeq (${TARGET_CPU}, UNKNOWN)
PORTING_SUBDIRS = ${THIRD_PARTY_DIRS} ${ITE_PRIVATE_DIRS} eudev libpng fontconfig pixman cairo imagemagick v4l-utils 
else 
PORTING_SUBDIRS = ${THIRD_PARTY_DIRS} ${ITE_PRIVATE_DIRS} ${ITE_OPENCY_DIRS} ${ITE_ALG_DEPENDS_DIRS}
endif # UNKNOWN

INSTALL_DIR := ${PLATFORM_RELEASE_DIRECTORY}

all:
	for i in $(PORTING_SUBDIRS);do \
		echo "Build $$i...."; ($(MAKE) -C $$i); \
		if [ $$? != 0 ] ; then \
			exit 1; \
		fi; \
	done

install:
	for i in $(PORTING_SUBDIRS);do \
		echo "Build $$i...."; ($(MAKE) -C $$i install); \
		if [ $$? != 0 ] ; then \
			exit 1; \
		fi; \
	done

clean:
	for i in $(PORTING_SUBDIRS);do \
		echo "Build $$i...."; rm -rf $$i/build; ($(MAKE) -C $$i clean); \
	done
