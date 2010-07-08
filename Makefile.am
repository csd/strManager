## top directory

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

MAINTAINERCLEANFILES = \
		Makefile.in \
		configure \
		aclocal.m4 \
		depcomp \
		config.sub \
		config.guess \
		install-sh \
		ltmain.sh \
		missing

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libstrmanager.pc


h_sources = 	include/libstrmanager/Manager.h \
		include/libstrmanager/Constants.h \
		include/libstrmanager/Message.h \
		include/libstrmanager/UdpStream.h \
		include/libstrmanager/StreamList.h \
		include/libstrmanager/StatsStreamList.h \
		include/libstrmanager/ForwardWorker.h \
		include/libstrmanager/StatsWorker.h \
		include/libstrmanager/RtcpWorker.h \
		include/libstrmanager/Worker.h
		 
cc_sources = 	src/Manager.cpp \
		src/Message.cpp \
		src/streams/UdpStream.cpp \
		src/streamlists/StreamList.cpp \
		src/streamlists/StatsStreamList.cpp \
		src/workers/ForwardWorker.cpp \
		src/workers/StatsWorker.cpp \
		src/workers/RtcpWorker.cpp \
		src/workers/Worker.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES= libstrmanager.la
libstrmanager_la_SOURCES= $(h_sources) $(cc_sources)
libstrmanager_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)

EXTRA_DIST = doc \
	     examples \
	     include/libstrmanager/Constants.h \
	     include/libstrmanager/ForwardWorker.h \
	     include/libstrmanager/Manager.h \
	     include/libstrmanager/Message.h \
	     include/libstrmanager/RtcpWorker.h \
	     include/libstrmanager/StatsStreamList.h \
	     include/libstrmanager/StatsWorker.h \
	     include/libstrmanager/StreamList.h \
	     include/libstrmanager/UdpStream.h \
	     include/libstrmanager/Worker.h \
	     strManager.doxyfile
