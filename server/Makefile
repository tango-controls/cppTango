#
#		Makefile to generate the Tango library
#

ifdef _solaris
CC = c++
AR = ar
AR_SL = $(CC) -fPIC -shared
BIN_DIR = solaris
ORBACUS_BASE = /segfs/tango/ORB/ORBacus4.0.3/solaris
JTC_BASE = /segfs/tango/JTC/solaris-1.0.12
SL_EXT = so
DB_INCLUDE_DIR = /segfs/tango/tmp/tango2/include/solaris7
#
OBJS_DIR_SRC = /segfs/tango/tmp/tango2/objs/arch/solaris7
OBJS_DIR_SRC_SL = /segfs/tango/tmp/tango2/objs/shared/solaris7
OBJS_DIR_DEST = api_objs/arch/solaris7
OBJS_DIR_DEST_SL = api_objs/shared/solaris7
endif

ifdef linux
CC = c++
AR = ar
AR_SL = $(CC) -fPIC -shared 
BIN_DIR = suse64
ORBACUS_BASE = /segfs/tango/ORB/ORBacus4.0.3/suse64
JTC_BASE = /segfs/tango/JTC/suse64-1.0.12
DB_INCLUDE_DIR = /segfs/tango/tmp/tango2/include/suse64
#
OBJS_DIR_SRC = /segfs/tango/tmp/tango2/objs/arch/suse64
OBJS_DIR_SRC_SL = /segfs/tango/tmp/tango2/objs/shared/suse64
OBJS_DIR_DEST = api_objs/arch/suse64
OBJS_DIR_DEST_SL = api_objs/shared/suse64
#
SL_EXT = so
DOC++ = /segfs/tango/doc/www/bin/Linux/doc++/bin/doc++
JAVA = /usr/lib/java/bin/java
DOC_TOOL_HOME = /segfs/tango/doc/www/bin/Linux/tool
DOC_HOME = /segfs/tango/cppserver/device/doc_html
GEN_DOC = $(DOC++) -p -H -B $(DOC_TOOL_HOME)/dummy -d doc_html
endif

ifdef __hpux10
CC = aCC
AR = ar
AR_SL = aCC -b
ORBACUS_BASE = /segfs/tango/ORB/ORBacus4.0.3/hpux10.2
JTC_BASE = /segfs/tango/JTC/hpux10.2-1.0.12
BIN_DIR = hpux10.2
SL_EXT = sl
DB_INCLUDE_DIR = /segfs/tango/tmp/tango2/include/hpux102
#
OBJS_DIR_SRC = /segfs/tango/tmp/tango2/objs/arch/hpux102
OBJS_DIR_SRC_SL = /segfs/tango/tmp/tango2/objs/shared/hpux102
OBJS_DIR_DEST = api_objs/arch/hpux102
OBJS_DIR_DEST_SL = api_objs/shared/hpux102
endif

INCLUDE_DIRS = -I $(ORBACUS_BASE)/include \
	       -I $(JTC_BASE)/include  \
	       -I . \
	       -I $(DB_INCLUDE_DIR)

# -----------------------------------------------------------------
#
#		The compiler flags
#
#------------------------------------------------------------------

ifdef _solaris
CXXFLAGS = -D_POSIX_PTHREAD_SEMANTICS $(INCLUDE_DIRS)
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
endif

ifdef linux
CXXFLAGS = -D_REENTRANT $(INCLUDE_DIRS)
CXXFLAGS_SL = $(CXXFLAGS) -fPIC
endif

ifdef __hpux
CXXFLAGS = -Aa -D_REENTRANT -D_CMA_NOWRAPPERS_ $(INCLUDE_DIRS)
CXXFLAGS_SL = $(CXXFLAGS) +z
endif

#-------------------------------------------------------------------

LIBNAME = libtango
AR_EXT = a

OBJS = 		tango_skel.o \
		tango.o \
		device.o \
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
		attrdesc.o \
		except.o \
		attrmanip.o
		
SRC_DOC =	device.h \
		command.h \
		utils.h \
		except.h \
		attrdesc.h \
		tango_const.h \
		attribute.h \
		multiattribute.h
		

OBJS_SL = 	tango_skel.so.o \
		tango.so.o \
		device.so.o \
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
		multiattribute.so.o \
		attrdesc.so.o \
		except.so.o \
		attrmanip.so.o
	
#
# Rule for archive libary
#

ifdef linux
.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c -o /tmp/$*.o $<
	mv -f /tmp/$*.o $@
else	
.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<
endif

#
# Rule for shared library
#

ifdef linux
.SUFFIXES:	.so.o .cpp
.cpp.so.o:
	$(CC) $(CXXFLAGS_SL) -c $< -o /tmp/$*.so.o
	mv -f /tmp/$*.so.o $*.so.o
else
.SUFFIXES:	.so.o .cpp
.cpp.so.o:
	$(CC) $(CXXFLAGS_SL) -c $< -o $*.so.o
endif
		
#-----------------------------------------------------------------


all:	$(LIBNAME).$(AR_EXT) $(LIBNAME).$(SL_EXT)
	
$(LIBNAME).$(AR_EXT): 	$(OBJS)
	$(AR) rv $(BIN_DIR)/$(LIBNAME).$(AR_EXT) $(OBJS) $(OBJS_DIR_DEST)/*.o

$(LIBNAME).$(SL_EXT):	$(OBJS_SL)
	$(AR_SL) -o $(BIN_DIR)/$(LIBNAME).$(SL_EXT) $(OBJS_SL) $(OBJS_DIR_DEST_SL)/*.so.o

doc:
	$(GEN_DOC) $(SRC_DOC)
	cd $(DOC_TOOL_HOME);$(JAVA) rem_inherited $(DOC_HOME)/TemplCommand.html											
	cd $(DOC_TOOL_HOME);$(JAVA) rem_inherited $(DOC_HOME)/TemplCommandIn.html											
	cd $(DOC_TOOL_HOME);$(JAVA) rem_inherited $(DOC_HOME)/TemplCommandOut.html
	cd $(DOC_TOOL_HOME);$(JAVA) rem_inherited $(DOC_HOME)/TemplCommandInOut.html
	cd $(DOC_TOOL_HOME);$(JAVA) rem_inherited $(DOC_HOME)/WAttribute.html
	cd $(DOC_TOOL_HOME);$(JAVA) rem_inherited $(DOC_HOME)/SpectrumAttr.html
	cd $(DOC_TOOL_HOME);$(JAVA) rem_inherited $(DOC_HOME)/ImageAttr.html
	rm -f $(DOC_HOME)/*_org

copy_objs:
	cp $(OBJS_DIR_SRC)/*.o $(OBJS_DIR_DEST)
	cd $(OBJS_DIR_DEST);rm -f $(OBJS);rm -f *.so.o
	cp $(OBJS_DIR_SRC_SL)/*.so.o $(OBJS_DIR_DEST_SL)
	cd $(OBJS_DIR_DEST_SL);rm -f $(OBJS_SL)
	
install_objs:
	cp $(OBJS) $(OBJS_DIR_SRC)
	cp $(OBJS_SL) $(OBJS_DIR_SRC_SL)
											
clean:
	rm -f *.o core
