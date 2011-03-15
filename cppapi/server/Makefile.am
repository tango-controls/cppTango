SUBDIRS=idl jpeg jpeg_mmx .

# We need the ORB to compile and the tango header files to compile
INCLUDES = -I$(top_srcdir)/lib/cpp/client \
	   -I$(top_srcdir)/lib/cpp/log4tango/include \
	   -I$(top_builddir)/lib/cpp/log4tango/include \
             $(ORB_INCLUDE_PREFIX)

# We're making a shared library with libtool (that's why we use LTLIBRARIES)
lib_LTLIBRARIES=libtango.la

# We need the libclient to link
libtango_la_LIBADD = ../client/libclient.la	\
						../log4tango/src/liblog4tango.la \
						jpeg/libjpeg.la \
						jpeg_mmx/libjpeg_mmx.la

# We need to set the -version-info for libtool so that libtool will
# generate the correct .so version
libtango_la_LDFLAGS=-version-info $(VERSION_INFO)

AM_CXXFLAGS=-D_TANGO_LIB

if DARWIN_ENABLED
AM_LDFLAGS=-flat_namespace --disable-dependency-tracking
endif

# The sources
libtango_la_SOURCES = tangoSK.cpp 		\
		      tangoDynSK.cpp 			\
		      attrdesc.cpp 				\
		      attribute.cpp 			\
		      attrmanip.cpp 			\
		      basiccommand.cpp 			\
		      blackbox.cpp 				\
		      class_factory.cpp 		\
		      classattribute.cpp 		\
		      command.cpp 				\
			  coutappender.cpp 			\
		      dev_event.cpp				\
		      device.cpp 				\
		      device_2.cpp 				\
		      device_3.cpp 				\
			  device_4.cpp 				\
		      deviceclass.cpp 			\
		      devicelog.cpp 			\
		      dserver.cpp 				\
		      dserverclass.cpp 			\
			  dserverlock.cpp			\
		      dserverlog.cpp 			\
		      dserverpoll.cpp 			\
		      dserversignal.cpp 		\
			  encoded_attribute.cpp		\
		      event.cpp 				\
		      eventcmds.cpp 			\
			  eventkeepalive.cpp		\
			  eventqueue.cpp			\
		      eventsupplier.cpp 		\
		      except.cpp 				\
		      logcmds.cpp 				\
		      logging.cpp 				\
		      logstream.cpp 			\
		      multiattribute.cpp 		\
		      pollcmds.cpp 				\
		      pollobj.cpp 				\
		      pollring.cpp 				\
		      pollthread.cpp  			\
		      seqvec.cpp 				\
			  subdev_diag.cpp			\
		      tangoappender.cpp 		\
		      tangorollingfileappender.cpp 	\
		      thsig.cpp 				\
		      utils.cpp 				\
			  utils_polling.cpp			\
			  utils_shut.cpp			\
		      w_attribute.cpp

tangoincludedir = $(includedir)/tango

tangoinclude_HEADERS = attrdesc.h		\
		  		  attribute.h		\
		  		  attrmanip.h		\
		  		  basiccommand.h	\
		  		  blackbox.h		\
		  		  classattribute.h	\
		  		  command.h			\
		  		  coutappender.h	\
		  		  coutbuf.h			\
		  		  device.h			\
		  		  device_2.h 		\
		  		  device_3.h		\
				  device_4.h		\
		  		  deviceclass.h		\
		  		  dserver.h			\
		  		  dserverclass.h	\
		  		  dserversignal.h	\
				  encoded_attribute.h \
				  encoded_format.h	\
		  		  event.h			\
		  		  eventcmds.h		\
		  		  eventsupplier.h	\
		  		  except.h			\
		  		  log4tango.h		\
		  		  logcmds.h			\
		  		  logging.h			\
		  		  logstream.h		\
		  		  multiattribute.h	\
		  		  ntservice.h		\
		  		  pollcmds.h		\
		  		  pollext.h			\
		  		  pollobj.h			\
		  		  pollring.h		\
		  		  pollthread.h		\
		  		  seqvec.h			\
				  subdev_diag.h		\
		  		  tango.h			\
		  		  tango_config.h	\
		  		  tango_const.h		\
		  		  tangoappender.h	\
		  		  tangorollingfileappender.h \
		  		  utils.h 			\
		  		  w_attribute.h		\
		  		  tango_monitor.h 	\
		  		  auto_tango_monitor.h \
		  		  readers_writers_lock.h

