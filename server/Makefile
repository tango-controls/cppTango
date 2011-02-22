#
#		Makefile to generate the Tango library
#

include Make.rules


API_SRC = ../client
API_INCLUDE_DIR = $(API_SRC)

ifdef _solaris7_gcc
CC = c++
AR = ar rv
AR_SL = $(CC) -fPIC -shared
VERS_OPT = -Wl,-h,
SL_EXT = so
#
API_OBJS = api_objs/arch/$(BIN_DIR)
API_OBJS_SL = api_objs/shared/$(BIN_DIR)
#
endif

ifdef _solaris9_gcc
CC = c++
AR = ar rv
AR_SL = $(CC) -fPIC -shared
VERS_OPT = -Wl,-h,
SL_EXT = so
#
API_OBJS = api_objs/arch/$(BIN_DIR)
API_OBJS_SL = api_objs/shared/$(BIN_DIR)
#
endif

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
API_OBJS = $(OBJS_DIR)/api_objs/arch/$(BIN_DIR)
API_OBJS_SL = $(OBJS_DIR_SL)/api_objs/shared/$(BIN_DIR)
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

OBJS_DIR = objs/$(BIN_DIR)_objs
OBJS_DIR_IDL = $(OBJS_DIR)
OBJS_DIR_SL = objs_sl/$(BIN_DIR)_objs
OBJS_DIR_IDL_SL = $(OBJS_DIR_SL)

ifdef 64bits
IDL_SRC = idl_64
else
IDL_SRC = idl
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

ifdef _solaris7_gcc
CXXFLAGS = -O2 -D_REENTRANT -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS 
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
endif

ifdef _solaris9_gcc
CXXFLAGS = -O2 -D_REENTRANT -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS 
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
endif


ifdef _solaris
CXXFLAGS = -mt -D_POSIX_PTHREAD_SEMANTICS -D_TANGO_LIB \
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
#CXXFLAGS = -g -D_REENTRANT -DOMNI_UNLOADABLE_STUBS $(INCLUDE_DIRS)
CXXFLAGS =  -D_REENTRANT -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
endif

ifdef hpux
CXXFLAGS = -AA -mt +Z -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS 
CXXFLAGS_SL = $(CXXFLAGS)
endif

#-------------------------------------------------------------------

LIBNAME = libtango
AR_EXT = a

IDL_OBJS_LIST =	$(OBJS_DIR_IDL)/tangoSK.o \
		$(OBJS_DIR_IDL)/tangoDynSK.o
			
OBJS = 		$(OBJS_DIR)/device.o \
		$(OBJS_DIR)/device_2.o \
		$(OBJS_DIR)/device_3.o \
		$(OBJS_DIR)/deviceclass.o \
		$(OBJS_DIR)/command.o \
		$(OBJS_DIR)/dserversignal.o \
		$(OBJS_DIR)/thsig.o \
		$(OBJS_DIR)/basiccommand.o \
		$(OBJS_DIR)/utils.o \
		$(OBJS_DIR)/dserverclass.o \
		$(OBJS_DIR)/dserver.o \
		$(OBJS_DIR)/class_factory.o \
		$(OBJS_DIR)/blackbox.o \
		$(OBJS_DIR)/classattribute.o \
		$(OBJS_DIR)/multiattribute.o \
		$(OBJS_DIR)/attribute.o \
		$(OBJS_DIR)/w_attribute.o \
		$(OBJS_DIR)/attrdesc.o \
		$(OBJS_DIR)/except.o \
		$(OBJS_DIR)/attrmanip.o \
		$(OBJS_DIR)/seqvec.o \
		$(OBJS_DIR)/pollring.o \
		$(OBJS_DIR)/pollobj.o \
		$(OBJS_DIR)/pollcmds.o \
		$(OBJS_DIR)/dserverpoll.o \
		$(OBJS_DIR)/pollthread.o \
		$(OBJS_DIR)/logging.o \
		$(OBJS_DIR)/logcmds.o \
		$(OBJS_DIR)/logstream.o \
		$(OBJS_DIR)/devicelog.o \
		$(OBJS_DIR)/dserverlog.o \
		$(OBJS_DIR)/coutappender.o \
		$(OBJS_DIR)/tangoappender.o \
		$(OBJS_DIR)/tangorollingfileappender.o \
		$(OBJS_DIR)/event.o \
		$(OBJS_DIR)/eventsupplier.o \
		$(OBJS_DIR)/eventcmds.o 
		
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


IDL_OBJS_SL_LIST =	$(OBJS_DIR_IDL_SL)/tangoSK.so.o \
			$(OBJS_DIR_IDL_SL)/tangoDynSK.so.o
						
OBJS_SL = 	$(OBJS_DIR_SL)/device.so.o \
		$(OBJS_DIR_SL)/device_2.so.o \
		$(OBJS_DIR_SL)/device_3.so.o \
		$(OBJS_DIR_SL)/deviceclass.so.o \
		$(OBJS_DIR_SL)/command.so.o \
		$(OBJS_DIR_SL)/dserversignal.so.o \
		$(OBJS_DIR_SL)/thsig.so.o \
		$(OBJS_DIR_SL)/basiccommand.so.o \
		$(OBJS_DIR_SL)/utils.so.o \
		$(OBJS_DIR_SL)/dserverclass.so.o \
		$(OBJS_DIR_SL)/dserver.so.o \
		$(OBJS_DIR_SL)/class_factory.so.o \
		$(OBJS_DIR_SL)/blackbox.so.o \
		$(OBJS_DIR_SL)/classattribute.so.o \
		$(OBJS_DIR_SL)/attribute.so.o \
		$(OBJS_DIR_SL)/w_attribute.so.o \
		$(OBJS_DIR_SL)/multiattribute.so.o \
		$(OBJS_DIR_SL)/attrdesc.so.o \
		$(OBJS_DIR_SL)/except.so.o \
		$(OBJS_DIR_SL)/attrmanip.so.o \
		$(OBJS_DIR_SL)/seqvec.so.o \
		$(OBJS_DIR_SL)/pollring.so.o \
		$(OBJS_DIR_SL)/pollobj.so.o \
		$(OBJS_DIR_SL)/pollcmds.so.o \
		$(OBJS_DIR_SL)/dserverpoll.so.o \
		$(OBJS_DIR_SL)/pollthread.so.o \
	  	$(OBJS_DIR_SL)/logcmds.so.o \
		$(OBJS_DIR_SL)/logstream.so.o \
	  	$(OBJS_DIR_SL)/devicelog.so.o \
	  	$(OBJS_DIR_SL)/logging.so.o \
	  	$(OBJS_DIR_SL)/dserverlog.so.o \
	  	$(OBJS_DIR_SL)/coutappender.so.o \
	  	$(OBJS_DIR_SL)/tangoappender.so.o \
	  	$(OBJS_DIR_SL)/tangorollingfileappender.so.o \
		$(OBJS_DIR_SL)/event.so.o \
		$(OBJS_DIR_SL)/eventsupplier.so.o \
		$(OBJS_DIR_SL)/eventcmds.so.o 
	  
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
$(OBJS_DIR)/%.o: %.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o

$(OBJS_DIR_IDL)/%.o: $(IDL_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR_IDL)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR_IDL)/$*.o
		
$(API_OBJS)/%.o: $(API_SRC)/%.cpp
	@./cr_dir $(API_OBJS)
	$(CC) $(CXXFLAGS) -c $< -o $(API_OBJS)/$*.o

$(OBJS_DIR_SL)/%.so.o: %.cpp
	@./cr_dir $(OBJS_DIR_SL)
	$(CC) $(CXXFLAGS_SL) -c $< -o $(OBJS_DIR_SL)/$*.so.o
	
$(OBJS_DIR_IDL_SL)/%.so.o: $(IDL_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR_IDL_SL)
	$(CC) $(CXXFLAGS_SL) -c $< -o $(OBJS_DIR_IDL_SL)/$*.so.o
	
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


													
clean:
	rm -f *.o core
	rm -f $(API_OBJS_SL)/*.o
	rm -f $(OBJS_DIR_SL)/*.o
	rm -f $(API_OBJS)/*.o
	rm -f $(OBJS_DIR)/*.o
		
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
	
