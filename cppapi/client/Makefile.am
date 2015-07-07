SUBDIRS=helpers .

# We need the ORB to compile and the tango header files to compile
AM_CPPFLAGS = -I$(top_srcdir)/lib/cpp/server \
              -I$(top_srcdir)/lib/cpp/log4tango/include \
              -I$(top_builddir)/lib/cpp/log4tango/include \
              -I$(top_builddir)/lib/cpp/server \
              $(ORB_INCLUDE_PREFIX) $(LIBZMQ_CFLAGS)

# We're making a shared library with libtool (That's why we use LTLIBRARIES)
lib_LTLIBRARIES = libtango.la

# We need the libserver to link
libtango_la_LIBADD = ../server/libserver.la	\
             ../server/idl/libidl.la \
		     ../server/jpeg/libjpeg.la \
		     ../server/jpeg_mmx/libjpeg_mmx.la \
		     ../log4tango/src/liblog4tango.la \
		     $(LIBZMQ_LIBS)

# We need to set the -version-info for libtool so that libtool will
# generate the correct .so version
libtango_la_LDFLAGS=-version-info $(VERSION_INFO)

AM_CXXFLAGS=-D_TANGO_LIB

if DARWIN_ENABLED
AM_LDFLAGS=-flat_namespace --disable-dependency-tracking
endif

# These are the sources for the library.
libtango_la_SOURCES =  dbapi_class.cpp         \
                       dbapi_server.cpp        \
                       dbapi_datum.cpp         \
                       dbapi_base.cpp          \
                       dbapi_device.cpp        \
                       dbapi_history.cpp       \
                       dbapi_attribute.cpp     \
                       dbapi_cache.cpp         \
                       dbapi_serverdata.cpp    \
                       devapi_attr.cpp         \
                       devapi_base.cpp         \
                       devapi_data.cpp         \
                       devapi_datahist.cpp     \
                       devapi_utils.cpp        \
                       devapi_pipe.cpp         \
                       api_util.cpp            \
                       asynreq.cpp             \
                       cbthread.cpp            \
                       proxy_asyn.cpp          \
                       proxy_asyn_cb.cpp       \
                       attr_proxy.cpp          \
                       group.cpp               \
                       filedatabase.cpp        \
                       apiexcept.cpp           \
                       accessproxy.cpp         \
                       lockthread.cpp          \
                       event.cpp               \
                       eventkeepalive.cpp      \
                       eventqueue.cpp          \
                       notifdeventconsumer.cpp \
                       zmqeventconsumer.cpp

tangoincludedir = $(includedir)/tango

tangoinclude_HEADERS = accessproxy.h        \
                       apiexcept.h          \
                       cbthread.h           \
                       dbapi.h              \
                       devapi.h             \
                       devasyn.h            \
                       event.h              \
                       eventconsumer.h      \
                       filedatabase.h       \
                       group.h              \
                       lockthread.h         \
                       Database.h           \
                       DbDevice.h           \
                       ApiUtil.h            \
                       DeviceData.h         \
                       DeviceAttribute.h    \
                       DevicePipe.h         \
                       Connection.h         \
                       DeviceProxy.h        \
                       AttributeProxy.h     \
                       event.tpp            \
                       devapi_attr.tpp      \
                       devapi_utils.tpp     \
                       api_util.tpp         \
                       devapi_pipe.tpp      \
                       zmq.hpp
