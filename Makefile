#
#		Makefile to generate the Tango library
#

include Make.rules


ifdef _solaris
ifdef gcc
CC = c++
AR = /usr/ccs/bin/ar rv
AR_SL = $(CC) -fPIC -shared -ldl
VERS_OPT = -Wl,-h,
SL_EXT = so
else
CC = /opt/SUNWspro/bin/CC
AR = $(CC) -xar -o 
AR_SL = $(CC) -mt -G -lCstd -ldl
VERS_OPT = -h
SL_EXT = so
endif
endif


ifdef linux
CC = c++
AR = ar rv
AR_SL = $(CC) -fPIC -shared -ldl 
VERS_OPT = -Wl,-soname,
SL_EXT = so
#
DOC_CONFIG = ../doxygen/Doxyfile
GEN_DOC = $(DOXYGEN) $(DOC_CONFIG)
endif


ifdef macosx-darwin8
CC = c++
AR = ar rv
#AR_SL = $(CC) -isysroot /DevelopAR_SLAR_SLper/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc -fPIC \
        -dynamiclib -flat_namespace --disable-dependency-tracking
AR_SL  = $(CC) -fPIC \
        -dynamiclib -flat_namespace --disable-dependency-tracking		  
SL_EXT = dylib
endif


ifdef hpux
CC = aCC
AR = ar rv
AR_SL = $(CC) -b
VERS_OPT = -Wl,+h,
SL_EXT = so
endif


CLIENT_SRC = client
SERVER_SRC = server
IDL_SRC    = server/idl
JPG_SRC    = server/jpeg

OBJS_DIR = 	objs/$(BIN_DIR)
OBJS_DIR_SL = 	objs_sl/$(BIN_DIR)


INCLUDE_DIRS = -I$(CLIENT_SRC) \
	       -I$(SERVER_SRC)	\
	       -I$(JPG_SRC) \
	       -I$(OMNI_BASE)/include \
	       -I$(LOG4TANGO_BASE)/include

# -----------------------------------------------------------------
#
#		The compiler flags
#
#------------------------------------------------------------------
MMFLAG =

ifdef _solaris
ifdef gcc
CXXFLAGS = -O2 -D_REENTRANT -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS 
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
else
CXXFLAGS = -mt -D_POSIX_PTHREAD_SEMANTICS -D_TANGO_LIB \
	   	  -DOMNI_UNLOADABLE_STUBS \
	        -xregs=no%appl $(INCLUDE_DIRS)
CXXFLAGS_SL = $(CXXFLAGS) -KPIC
endif
endif


ifdef linux
#FLAGS = -g -D_REENTRANT -DOMNI_UNLOADABLE_STUBS $(INCLUDE_DIRS)
FLAGS    = -D_REENTRANT -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS

# gcc does not allow MMX optimisation with -fPIC in 64bits
ifdef 64bits
MMFLAG      = -mmmx -D_64BITS
CXXFLAGS    = -DJPG_USE_ASM $(FLAGS)
CXXFLAGS_SL = $(FLAGS) -fPIC
else
MMFLAG      = -mmmx
CXXFLAGS    = -DJPG_USE_ASM $(FLAGS)
CXXFLAGS_SL = -DJPG_USE_ASM $(FLAGS) -fPIC
endif

endif

ifdef macosx-darwin8
#CXXFLAGS = -g -isysroot /Developper/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc -D_REENTRANT -DOMNI_UNLOADABLE_STUBS $(INCLUDE_DIRS)
CXXFLAGS = -g -D_REENTRANT -DOMNI_UNLOADABLE_STUBS $(INCLUDE_DIRS)
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
endif

ifdef hpux
CXXFLAGS = -AA -mt +Z -D_TANGO_LIB $(INCLUDE_DIRS) -DOMNI_UNLOADABLE_STUBS 
CXXFLAGS_SL = $(CXXFLAGS)
endif

#-------------------------------------------------------------------

LIBNAME = libtango
AR_EXT = a

IDL_OBJS =	$(OBJS_DIR)/tangoSK.o \
		$(OBJS_DIR)/tangoDynSK.o
			
SERVER_OBJS = 	$(OBJS_DIR)/device.o \
		$(OBJS_DIR)/device_2.o \
		$(OBJS_DIR)/device_3.o \
		$(OBJS_DIR)/device_4.o \
		$(OBJS_DIR)/dev_event.o \
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
		$(OBJS_DIR)/dserverlock.o \
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
		$(OBJS_DIR)/eventcmds.o \
		$(OBJS_DIR)/eventqueue.o \
		$(OBJS_DIR)/eventkeepalive.o \
		$(OBJS_DIR)/utils_polling.o \
		$(OBJS_DIR)/utils_shut.o \
		$(OBJS_DIR)/subdev_diag.o \
		$(OBJS_DIR)/encoded_attribute.o \
		$(OBJS_DIR)/jpeg_bitstream.o \
		$(OBJS_DIR)/jpeg_color.o \
		$(OBJS_DIR)/jpeg_color_mmx.o \
		$(OBJS_DIR)/jpeg_dct.o \
		$(OBJS_DIR)/jpeg_dct_mmx.o \
		$(OBJS_DIR)/jpeg_decoder.o \
		$(OBJS_DIR)/jpeg_encoder.o \
		$(OBJS_DIR)/jpeg_memory.o
				
CLIENT_OBJS = 	$(OBJS_DIR)/dbapi_base.o \
		$(OBJS_DIR)/dbapi_history.o \
		$(OBJS_DIR)/dbapi_class.o \
		$(OBJS_DIR)/dbapi_datum.o \
		$(OBJS_DIR)/dbapi_device.o \
		$(OBJS_DIR)/dbapi_server.o \
		$(OBJS_DIR)/dbapi_cache.o \
		$(OBJS_DIR)/devapi_attr.o \
		$(OBJS_DIR)/devapi_base.o \
		$(OBJS_DIR)/devapi_data.o \
		$(OBJS_DIR)/devapi_datahist.o \
		$(OBJS_DIR)/api_util.o \
		$(OBJS_DIR)/devapi_utils.o \
		$(OBJS_DIR)/asynreq.o \
		$(OBJS_DIR)/proxy_asyn.o \
		$(OBJS_DIR)/proxy_asyn_cb.o \
		$(OBJS_DIR)/cbthread.o \
		$(OBJS_DIR)/lockthread.o \
		$(OBJS_DIR)/group.o \
		$(OBJS_DIR)/accessproxy.o \
		$(OBJS_DIR)/dbapi_attribute.o \
		$(OBJS_DIR)/attr_proxy.o \
		$(OBJS_DIR)/apiexcept.o \
		$(OBJS_DIR)/filedatabase.o

IDL_OBJS_SL =	$(OBJS_DIR_SL)/tangoSK.so.o \
		$(OBJS_DIR_SL)/tangoDynSK.so.o
						
SERVER_OBJS_SL =$(OBJS_DIR_SL)/device.so.o \
		$(OBJS_DIR_SL)/device_2.so.o \
		$(OBJS_DIR_SL)/device_3.so.o \
		$(OBJS_DIR_SL)/device_4.so.o \
		$(OBJS_DIR_SL)/dev_event.so.o \
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
		$(OBJS_DIR_SL)/dserverlock.so.o \
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
		$(OBJS_DIR_SL)/eventcmds.so.o \
		$(OBJS_DIR_SL)/eventqueue.so.o \
		$(OBJS_DIR_SL)/eventkeepalive.so.o \
		$(OBJS_DIR_SL)/utils_polling.so.o \
		$(OBJS_DIR_SL)/utils_shut.so.o \
		$(OBJS_DIR_SL)/subdev_diag.so.o \
		$(OBJS_DIR_SL)/encoded_attribute.so.o \
		$(OBJS_DIR_SL)/jpeg_bitstream.so.o \
		$(OBJS_DIR_SL)/jpeg_color.so.o \
		$(OBJS_DIR_SL)/jpeg_color_mmx.so.o \
		$(OBJS_DIR_SL)/jpeg_dct.so.o \
		$(OBJS_DIR_SL)/jpeg_dct_mmx.so.o \
		$(OBJS_DIR_SL)/jpeg_decoder.so.o \
		$(OBJS_DIR_SL)/jpeg_encoder.so.o \
		$(OBJS_DIR_SL)/jpeg_memory.so.o
	  
CLIENT_OBJS_SL = $(OBJS_DIR_SL)/dbapi_base.so.o \
		$(OBJS_DIR_SL)/dbapi_history.so.o \
		$(OBJS_DIR_SL)/dbapi_class.so.o \
		$(OBJS_DIR_SL)/dbapi_datum.so.o \
		$(OBJS_DIR_SL)/dbapi_device.so.o \
		$(OBJS_DIR_SL)/dbapi_server.so.o \
		$(OBJS_DIR_SL)/dbapi_cache.so.o \
		$(OBJS_DIR_SL)/devapi_attr.so.o \
		$(OBJS_DIR_SL)/devapi_base.so.o \
		$(OBJS_DIR_SL)/devapi_data.so.o \
		$(OBJS_DIR_SL)/devapi_datahist.so.o \
		$(OBJS_DIR_SL)/api_util.so.o \
		$(OBJS_DIR_SL)/devapi_utils.so.o \
		$(OBJS_DIR_SL)/asynreq.so.o \
		$(OBJS_DIR_SL)/proxy_asyn.so.o \
		$(OBJS_DIR_SL)/proxy_asyn_cb.so.o \
		$(OBJS_DIR_SL)/cbthread.so.o \
		$(OBJS_DIR_SL)/lockthread.so.o \
		$(OBJS_DIR_SL)/group.so.o \
		$(OBJS_DIR_SL)/accessproxy.so.o \
		$(OBJS_DIR_SL)/dbapi_attribute.so.o \
		$(OBJS_DIR_SL)/attr_proxy.so.o \
		$(OBJS_DIR_SL)/apiexcept.so.o \
		$(OBJS_DIR_SL)/filedatabase.so.o

IDL_INCLUDE    = 	tango.h

HELPERS_INCLUDE =	DeviceProxyHelper.h \
			PogoHelper.h \
			TangoExceptionsHelper.h \
			Xstring.h
						
CLIENT_INCLUDE =	apiexcept.h \
			cbthread.h \
			lockthread.h \
			dbapi.h \
			devapi.h \
			devasyn.h \
			filedatabase.h \
			group.h \
			accessproxy.h
						
SERVER_INCLUDE =	attrdesc.h \
			attribute.h \
			attrmanip.h \
			auto_tango_monitor.h \
			basiccommand.h \
			blackbox.h \
			classattribute.h \
			command.h \
			coutappender.h \
			coutbuf.h \
			device.h \
			device_2.h \
			device_3.h \
			device_4.h \
			deviceclass.h \
			dserver.h \
			dserverclass.h \
			dserversignal.h \
			event.h \
			eventcmds.h \
			eventsupplier.h \
			except.h \
			log4tango.h \
			logcmds.h \
			logging.h \
			logstream.h \
			multiattribute.h \
			ntservice.h \
			pollcmds.h \
			pollext.h \
			pollobj.h \
			pollring.h \
			pollthread.h \
			readers_writers_lock.h \
			seqvec.h \
			tango.h \
			tango_config.h \
			tango_const.h \
			tango_monitor.h \
			tangoappender.h \
			tangorollingfileappender.h \
			utils.h \
			w32win.h \
			w_attribute.h \
			subdev_diag.h \
			encoded_attribute.h
				

#-----------------------------------------------------------------

all:	$(LIBNAME).$(AR_EXT) $(LIBNAME).$(SL_EXT) install_include install_link

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
$(OBJS_DIR)/%.o: $(SERVER_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o

$(OBJS_DIR)/%.o: $(IDL_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o

$(OBJS_DIR)/%.o: $(JPG_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o
		
$(OBJS_DIR)/%.o: $(CLIENT_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o

$(OBJS_DIR)/jpeg_color_mmx.o: $(JPG_SRC)/jpeg_color_mmx.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) $(MMFLAG) -c $(JPG_SRC)/jpeg_color_mmx.cpp -o $(OBJS_DIR)/jpeg_color_mmx.o

$(OBJS_DIR)/jpeg_dct_mmx.o: $(JPG_SRC)/jpeg_dct_mmx.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) $(MMFLAG) -c $(JPG_SRC)/jpeg_dct_mmx.cpp -o $(OBJS_DIR)/jpeg_dct_mmx.o

#-----------------------------------------------------------------

$(OBJS_DIR_SL)/%.so.o: $(SERVER_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR_SL)
	$(CC) $(CXXFLAGS_SL) -c $< -o $(OBJS_DIR_SL)/$*.so.o
	
$(OBJS_DIR_SL)/%.so.o: $(IDL_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR_SL)
	$(CC) $(CXXFLAGS_SL) -c $< -o $(OBJS_DIR_SL)/$*.so.o

$(OBJS_DIR_SL)/%.so.o: $(JPG_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR_SL)
	$(CC) $(CXXFLAGS_SL) -c $< -o $(OBJS_DIR_SL)/$*.so.o
	
$(OBJS_DIR_SL)/%.so.o: $(CLIENT_SRC)/%.cpp
	@./cr_dir $(OBJS_DIR_SL)
	$(CC) $(CXXFLAGS_SL) -c $< -o $(OBJS_DIR_SL)/$*.so.o

$(OBJS_DIR_SL)/jpeg_color_mmx.so.o: $(JPG_SRC)/jpeg_color_mmx.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(MMFLAG) $(CXXFLAGS_SL) -c $(JPG_SRC)/jpeg_color_mmx.cpp -o $(OBJS_DIR_SL)/jpeg_color_mmx.so.o

$(OBJS_DIR_SL)/jpeg_dct_mmx.so.o: $(JPG_SRC)/jpeg_dct_mmx.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(MMFLAG) $(CXXFLAGS_SL) -c $(JPG_SRC)/jpeg_dct_mmx.cpp -o $(OBJS_DIR_SL)/jpeg_dct_mmx.so.o

					
#-----------------------------------------------------------------
#
#	The archive libs
#

$(LIBNAME).$(AR_EXT): 	$(IDL_OBJS) $(SERVER_OBJS) $(CLIENT_OBJS)
	@./cr_dir $(INSTALL_BASE)/lib
	$(AR) $(INSTALL_BASE)/lib/$(LIBNAME).$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).$(AR_EXT) \
	$(IDL_OBJS) $(SERVER_OBJS) $(CLIENT_OBJS)

#
#	The shared libs
#

ifndef macosx-darwin8
$(LIBNAME).$(SL_EXT):	$(IDL_OBJS_SL) $(SERVER_OBJS_SL) $(CLIENT_OBJS_SL)
	@./cr_dir $(INSTALL_BASE)/lib
	$(AR_SL) -o $(INSTALL_BASE)/lib/$(LIBNAME).$(SL_EXT).$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS) \
	$(VERS_OPT)$(LIBNAME).$(SL_EXT).$(MAJOR_VERS) \
	$(IDL_OBJS_SL) $(SERVER_OBJS_SL) $(CLIENT_OBJS_SL)

install_link:
	d=`pwd`
	cd $(INSTALL_BASE)/lib; \
	rm $(LIBNAME).$(AR_EXT); ln -s $(LIBNAME).$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).$(AR_EXT) $(LIBNAME).$(AR_EXT); \
	rm $(LIBNAME).$(SL_EXT); ln -s $(LIBNAME).$(SL_EXT).$(MAJOR_VERS) $(LIBNAME).$(SL_EXT); \
	rm $(LIBNAME).$(SL_EXT).$(MAJOR_VERS); ln -s $(LIBNAME).$(SL_EXT).$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS) $(LIBNAME).$(SL_EXT).$(MAJOR_VERS); \
	cd $d

else
# MacOSX has to link the shared library
$(LIBNAME).$(SL_EXT):	$(IDL_OBJS_SL) $(SERVER_OBJS_SL) $(CLIENT_OBJS_SL)
	@./cr_dir $(INSTALL_BASE)/lib
	$(AR_SL) \
	-o $(INSTALL_BASE)/lib/$(LIBNAME).$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).$(SL_EXT) \
	-install_name $(INSTALL_BASE)/lib/$(LIBNAME).$(MAJOR_VERS).$(SL_EXT) \
	$(IDL_OBJS_SL) $(SERVER_OBJS_SL) $(CLIENT_OBJS_SL) \
	-L$(OMNI_BASE)/lib -L$(LOG4TANGO_BASE)/lib \
	-llog4tango             \
	-lomniORB4              \
	-lomniDynamic4  			\
	-lomnithread    			\
	-lCOS4

# library verioning is not handled the same way under MacOSX
install_link:
	d=`pwd`
	cd $(INSTALL_BASE)/lib; \
	rm $(LIBNAME).$(AR_EXT); ln -s $(LIBNAME).$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).$(AR_EXT) $(LIBNAME).$(AR_EXT); \
	rm $(LIBNAME).$(SL_EXT); ln -s $(LIBNAME).$(MAJOR_VERS).$(SL_EXT) $(LIBNAME).$(SL_EXT); \
	rm $(LIBNAME).$(MAJOR_VERS).$(SL_EXT); ln -s $(LIBNAME).$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).$(SL_EXT) $(LIBNAME).$(MAJOR_VERS).$(SL_EXT); \
	cd $d
endif
	
install_include:
	@./cr_dir $(INSTALL_BASE)/include
	@./cr_dir $(INSTALL_BASE)/include/idl
	cd server/idl; cp $(IDL_INCLUDE) $(INSTALL_BASE)/include/idl; cd ../..
	cd server; cp $(SERVER_INCLUDE) $(INSTALL_BASE)/include; cd ..
	cd client/helpers; cp $(HELPERS_INCLUDE) $(INSTALL_BASE)/include; cd ../..
	cd client; cp $(CLIENT_INCLUDE) $(INSTALL_BASE)/include; cd ..

install_win32:
	@./cr_dir $(INSTALL_BASE_WIN32)/include
	@./cr_dir $(INSTALL_BASE_WIN32)/include/idl
	cd server/idl; cp $(IDL_INCLUDE) $(INSTALL_BASE_WIN32)/include/idl; cd ../..
	cd server; cp $(SERVER_INCLUDE) $(INSTALL_BASE_WIN32)/include; cd ..
	cd client_helpers; cp $(HELPERS_INCLUDE) $(INSTALL_BASE_WIN32)/include; cd ../..
	cd client; cp $(CLIENT_INCLUDE) $(INSTALL_BASE_WIN32)/include; cd ..	
	@./cr_dir $(INSTALL_BASE_WIN32)_dll/include
	@./cr_dir $(INSTALL_BASE_WIN32)_dll/include/idl
	cd server/idl; cp $(IDL_INCLUDE) $(INSTALL_BASE_WIN32)_dll/include/idl; cd ../..
	cd server; cp $(SERVER_INCLUDE) $(INSTALL_BASE_WIN32)_dll/include; cd ..
	cd client; cp $(CLIENT_INCLUDE) $(INSTALL_BASE_WIN32)_dll/include; cd ..
	
	@./cr_dir $(INSTALL_BASE_WIN32)/lib
	cp win32/winnt_lib/tango_static/lib/tangod.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib $(INSTALL_BASE_WIN32)/lib
	cp win32/winnt_lib/tango_static/lib/tangod.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).pdb $(INSTALL_BASE_WIN32)/lib
	cp win32/winnt_lib/tango_static/lib/tango.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib $(INSTALL_BASE_WIN32)/lib

	@./cr_dir $(INSTALL_BASE_WIN32)_dll/lib
	cp win32/winnt_lib/tango_dll/lib/tangod.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib $(INSTALL_BASE_WIN32)_dll/lib
	cp win32/winnt_lib/tango_dll/lib/tangod.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).pdb $(INSTALL_BASE_WIN32)_dll/lib
	cp win32/winnt_lib/tango_dll/lib/tango$(MAJOR_VERS)$(MINOR_VERS)$(PATCH_VERS)d.dll $(INSTALL_BASE_WIN32)_dll/lib
	cp win32/winnt_lib/tango_dll/lib/tango.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib $(INSTALL_BASE_WIN32)_dll/lib
	cp win32/winnt_lib/tango_dll/lib/tango$(MAJOR_VERS)$(MINOR_VERS)$(PATCH_VERS).dll $(INSTALL_BASE_WIN32)_dll/lib

	d=`pwd`
	cd $(INSTALL_BASE_WIN32)/lib; \
	rm tangod.lib; ln -s tangod.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib tangod.lib; \
	rm tango.lib; ln -s tango.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib tango.lib; \

	cd $(INSTALL_BASE_WIN32)_dll/lib; \
	rm tangod.lib; ln -s tangod.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib tangod.lib; \
	rm tango.lib; ln -s tango.$(MAJOR_VERS).$(MINOR_VERS).$(PATCH_VERS).lib tango.lib;	\
	cd $d

	
doc:
	cd server; $(GEN_DOC); cd ..

clean:
	rm -f *.o core
	rm -f $(OBJS_DIR_SL)/*.o
	rm -f $(OBJS_DIR)/*.o

clean_all:
	rm -f *.o
	rm -f test/*.html
	rm -f test/solaris/*.html
	rm -f test/suse72/*.html
	rm -f test/*.log
	rm -f test/solaris/*.log
	rm -f test/suse72/*log
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
	
