#
#		Makefile to generate the Tango library
#

include Make.rules


API_SRC = ../client
IDL_SRC = idl
API_INCLUDE_DIR = $(API_SRC)

ifdef _solaris
CC = CC
AR = CC -xar -o 
AR_SL = $(CC) -mt -G
VERS_OPT = -h
SL_EXT = so
#
API_OBJS = api_objs/arch/$(BIN_DIR)
API_OBJS_SL = api_objs/shared/$(BIN_DIR)
#
endif

ifdef _solaris9
CC = CC
AR = CC -xar -o 
AR_SL = $(CC) -mt -G
VERS_OPT = -h
SL_EXT = so
#
API_OBJS = api_objs/arch/$(BIN_DIR)
API_OBJS_SL = api_objs/shared/$(BIN_DIR)
#
endif

ifdef linux
CC = c++
AR = ar rv
AR_SL = $(CC) -fPIC -shared 
VERS_OPT = -Wl,-soname,
#
API_OBJS = api_objs/arch/$(BIN_DIR)
API_OBJS_SL = api_objs/shared/$(BIN_DIR)
#
SL_EXT = so
#
DOXYGEN = $(TANGO_HOME)/bin/doxygen
DOC_CONFIG = doxygen/Doxyfile
GEN_DOC = $(DOXYGEN) $(DOC_CONFIG)
endif


ifdef hpux
CC = aCC
AR = ar rv
AR_SL = $(CC) -b
VERS_OPT = -Wl,+h,
#
API_OBJS = api_objs/arch/$(BIN_DIR)
API_OBJS_SL = api_objs/shared/$(BIN_DIR)
#
SL_EXT = so
#
endif

INCLUDE_DIRS = -I$(OMNI_BASE)/include \
	       -I$(LOG4TANGO_BASE)/include \
	       -I. \
	       -I$(API_INCLUDE_DIR)
	       
# -----------------------------------------------------------------
#
#		The compiler flags
#
#------------------------------------------------------------------

ifdef _solaris
CXXFLAGS = -mt -g -D_POSIX_PTHREAD_SEMANTICS -D_TANGO_LIB \
	   -DOMNI_UNLOADABLE_STUBS \
	   -xregs=no%appl $(INCLUDE_DIRS)
CXXFLAGS_SL = $(CXXFLAGS) -KPIC
endif

ifdef _solaris9
CXXFLAGS = -mt -D_POSIX_PTHREAD_SEMANTICS -D_TANGO_LIB \
	   -DOMNI_UNLOADABLE_STUBS \
	   -xregs=no%appl $(INCLUDE_DIRS)
CXXFLAGS_SL = $(CXXFLAGS) -KPIC
endif

ifdef linux
CXXFLAGS = -g -D_REENTRANT -DOMNI_UNLOADABLE_STUBS $(INCLUDE_DIRS)
#CXXFLAGS = -O2 -D_REENTRANT -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS 
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
endif

ifdef hpux
CXXFLAGS = -AA -mt +Z -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS 
CXXFLAGS_SL = $(CXXFLAGS)
endif

#-------------------------------------------------------------------

LIBNAME = libtango
AR_EXT = a

IDL_OBJS_LIST =	tangoSK.o \
		tangoDynSK.o
			
OBJS = 		device.o \
		device_2.o \
		device_3.o \
		deviceclass.o \
		command.o \
		dserversignal.o \
		thsig.o \
		basiccommand.o \
		utils.o \
		dserverclass.o \
		dserver.o \
		class_factory.o \
		blackbox.o \
		classattribute.o \
		multiattribute.o \
		attribute.o \
		w_attribute.o \
		attrdesc.o \
		except.o \
		attrmanip.o \
		seqvec.o \
		pollring.o \
		pollobj.o \
		pollcmds.o \
		dserverpoll.o \
		pollthread.o \
		logging.o \
		logcmds.o \
		logstream.o \
		devicelog.o \
		dserverlog.o \
		coutappender.o \
		tangoappender.o \
		tangorollingfileappender.o \
		event.o \
		eventsupplier.o \
		eventcmds.o 
		
API_OBJS_LIST = $(API_OBJS)/dbapi_base.o \
		$(API_OBJS)/dbapi_class.o \
		$(API_OBJS)/dbapi_datum.o \
		$(API_OBJS)/dbapi_device.o \
		$(API_OBJS)/dbapi_server.o \
		$(API_OBJS)/devapi_attr.o \
		$(API_OBJS)/devapi_base.o \
		$(API_OBJS)/devapi_data.o \
		$(API_OBJS)/devapi_datahist.o \
		$(API_OBJS)/api_util.o \
		$(API_OBJS)/asynreq.o \
		$(API_OBJS)/proxy_asyn.o \
		$(API_OBJS)/proxy_asyn_cb.o \
		$(API_OBJS)/cbthread.o \
		$(API_OBJS)/group.o \
		$(API_OBJS)/dbapi_attribute.o \
		$(API_OBJS)/attr_proxy.o \
		$(API_OBJS)/apiexcept.o \
		$(API_OBJS)/filedatabase.o 


IDL_OBJS_SL_LIST =	tangoSK.so.o \
			tangoDynSK.so.o
						
OBJS_SL = 	device.so.o \
		device_2.so.o \
		device_3.so.o \
		deviceclass.so.o \
		command.so.o \
		dserversignal.so.o \
		thsig.so.o \
		basiccommand.so.o \
		utils.so.o \
		dserverclass.so.o \
		dserver.so.o \
		class_factory.so.o \
		blackbox.so.o \
		classattribute.so.o \
		attribute.so.o \
		w_attribute.so.o \
		multiattribute.so.o \
		attrdesc.so.o \
		except.so.o \
		attrmanip.so.o \
		seqvec.so.o \
		pollring.so.o \
		pollobj.so.o \
		pollcmds.so.o \
		dserverpoll.so.o \
		pollthread.so.o \
	  	logcmds.so.o \
		logstream.so.o \
	  	devicelog.so.o \
	  	logging.so.o \
	  	dserverlog.so.o \
	  	coutappender.o \
	  	tangoappender.so.o \
	  	tangorollingfileappender.so.o \
		event.so.o \
		eventsupplier.so.o \
		eventcmds.so.o 
	  
API_OBJS_SL_LIST = $(API_OBJS_SL)/dbapi_base.so.o \
		$(API_OBJS_SL)/dbapi_class.so.o \
		$(API_OBJS_SL)/dbapi_datum.so.o \
		$(API_OBJS_SL)/dbapi_device.so.o \
		$(API_OBJS_SL)/dbapi_server.so.o \
		$(API_OBJS_SL)/devapi_attr.so.o \
		$(API_OBJS_SL)/devapi_base.so.o \
		$(API_OBJS_SL)/devapi_data.so.o \
		$(API_OBJS_SL)/devapi_datahist.so.o \
		$(API_OBJS_SL)/api_util.so.o \
		$(API_OBJS_SL)/asynreq.so.o \
		$(API_OBJS_SL)/proxy_asyn.so.o \
		$(API_OBJS_SL)/proxy_asyn_cb.so.o \
		$(API_OBJS_SL)/cbthread.so.o \
		$(API_OBJS_SL)/group.so.o \
		$(API_OBJS_SL)/dbapi_attribute.so.o \
		$(API_OBJS_SL)/attr_proxy.so.o \
		$(API_OBJS_SL)/apiexcept.so.o \
		$(API_OBJS_SL)/filedatabase.so.o
	
#
# Rule for archive libary
#
	
.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<

#
# Rule for shared library
#

.SUFFIXES:	.so.o .cpp
.cpp.so.o:
	$(CC) $(CXXFLAGS_SL) -c $< -o $*.so.o

#
# Rule for API files
#

$(API_OBJS)/%.o: $(API_SRC)/%.cpp
	@./cr_dir $(API_OBJS)
	$(CC) $(CXXFLAGS) -c $< -o $(API_OBJS)/$*.o

$(API_OBJS_SL)/%.so.o: $(API_SRC)/%.cpp
	@./cr_dir $(API_OBJS_SL)
	$(CC) $(CXXFLAGS_SL) -c $< -o $(API_OBJS_SL)/$*.so.o
					
#-----------------------------------------------------------------


all:	$(LIBNAME).$(AR_EXT) $(LIBNAME).$(SL_EXT)

#
#	The archive libs
#

$(LIBNAME).$(AR_EXT): 	$(IDL_OBJS_LIST) $(OBJS) $(API_OBJS_LIST)
	@./cr_dir $(BIN_DIR)
	$(AR) $(BIN_DIR)/$(LIBNAME).$(AR_EXT) \
	$(IDL_OBJS_LIST) $(OBJS) $(API_OBJS_LIST)

#
#	The shared libs
#

$(LIBNAME).$(SL_EXT):	$(IDL_OBJS_SL_LIST) $(OBJS_SL) $(API_OBJS_SL_LIST)
	@./cr_dir $(BIN_DIR)
	$(AR_SL) -o $(BIN_DIR)/$(LIBNAME).$(SL_EXT).$(MAJOR_VERS).$(MINOR_VERS) \
	$(VERS_OPT)$(LIBNAME).$(SL_EXT).$(MAJOR_VERS) \
	$(IDL_OBJS_SL_LIST) $(OBJS_SL) $(API_OBJS_SL_LIST)

doc:
	$(GEN_DOC)


tangoSK.so.o: $(IDL_SRC)/tangoSK.cpp
	cd $(IDL_SRC);$(CC) $(CXXFLAGS_SL) -c tangoSK.cpp -o ../tangoSK.so.o

tangoDynSK.so.o: $(IDL_SRC)/tangoDynSK.cpp
	cd $(IDL_SRC);$(CC) $(CXXFLAGS_SL) -c tangoDynSK.cpp -o ../tangoDynSK.so.o

tangoSK.o: $(IDL_SRC)/tangoSK.cpp
	cd $(IDL_SRC);$(CC) $(CXXFLAGS) -c tangoSK.cpp -o ../tangoSK.o

tangoDynSK.o: $(IDL_SRC)/tangoDynSK.cpp
	cd $(IDL_SRC);$(CC) $(CXXFLAGS) -c tangoDynSK.cpp -o ../tangoDynSK.o
													
clean:
	rm -f *.o core
	rm -f api_objs/arch/*/*.o
	rm -f api_objs/shared/*/*.o
	
clean_all:
	rm -f *.o
	rm -f api_objs/arch/*/*.o
	rm -f api_objs/shared/*/*.o
	rm -f test/*.html
	rm -f test/solaris/*.html
	rm -f test/suse72/*.html
	rm -f test/*.log
	rm -f test/solaris/*.log
	rm -f test/suse72/*log
	rm -f solaris/*.a
	rm -f solaris/*.so
	rm -f solaris7_CC/*.a
	rm -f solaris7_CC/*.so
	rm -f suse72/*.a
	rm -f suse72/*.so
	rm -f winnt_lib/tango_static/Debug/*.obj
	rm -f winnt_lib/tango_static/Debug/*.lib
	rm -f winnt_lib/tango_static/Release/*.obj
	rm -f winnt_lib/tango_static/Release/*.lib
	rm -f winnt_lib/tango_dll/Debug/*.obj
	rm -f winnt_lib/tango_dll/Debug/*.lib
	rm -f winnt_lib/tango_dll/Debug/*.dll
	rm -f winnt_lib/tango_dll/Debug/*.exp
	rm -f winnt_lib/tango_dll/Debug/*.map
	rm -f winnt_lib/tango_dll/Release/*.obj
	rm -f winnt_lib/tango_dll/Release/*.lib
	rm -f winnt_lib/tango_dll/Release/*.dll
	rm -f winnt_lib/tango_dll/Release/*.exp
	rm -f winnt_lib/tango_dll/Release/*.map
	
