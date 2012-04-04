SUBDIRS=helpers .

# We need the ORB to compile and the tango header files to compile
INCLUDES = -I$(top_srcdir)/lib/cpp/server \
           -I$(top_srcdir)/lib/cpp/log4tango/include \
	   	   -I$(top_builddir)/lib/cpp/log4tango/include \
             $(ORB_INCLUDE_PREFIX) $(LIBZMQ_CFLAGS)

# We're making a libtool convenience library which is not to be installed,
# therefore the automake noinst variable
noinst_LTLIBRARIES = libclient.la

AM_CXXFLAGS=-D_TANGO_LIB @CPP_ELEVEN@

# These are the sources for the library.
libclient_la_SOURCES = dbapi_class.cpp     	\
                       dbapi_server.cpp    	\
                       dbapi_datum.cpp     	\
                       dbapi_base.cpp      	\
                       dbapi_device.cpp    	\
					   dbapi_history.cpp   	\
					   dbapi_attribute.cpp 	\
					   dbapi_cache.cpp		\
                       devapi_attr.cpp     	\
                       devapi_base.cpp     	\
                       devapi_data.cpp     	\
                       devapi_datahist.cpp 	\
					   devapi_utils.cpp		\
                       api_util.cpp        	\
                       asynreq.cpp	   		\
                       cbthread.cpp	   		\
                       proxy_asyn.cpp	   	\
                       proxy_asyn_cb.cpp   	\
					   attr_proxy.cpp      	\
					   group.cpp	   		\
					   filedatabase.cpp    	\
					   apiexcept.cpp 		\
					   accessproxy.cpp		\
					   lockthread.cpp		\
					   event.cpp			\
					   eventkeepalive.cpp 	\
					   eventqueue.cpp		\
					   notifdeventconsumer.cpp \
					   zmqeventconsumer.cpp

					   

tangoincludedir = $(includedir)/tango

tangoinclude_HEADERS = accessproxy.h		\
				  apiexcept.h 		\
		  		  cbthread.h  		\
		  		  dbapi.h     		\
		  		  devapi.h    		\
		  		  devasyn.h   		\
				  event.h			\
				  eventconsumer.h	\
		  		  filedatabase.h    \
		  		  group.h			\
				  lockthread.h
