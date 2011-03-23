#
#		Makefile to generate a Tango server
#

##############################################################

#
# Only these two variables TANGO_HOME and LIB_TANGO_DIR
# should be modified to compile on several places
# at the ESRF
#

################################################################

#TANGO_HOME = /home/taurel/tango/cppapi
#TANGO_HOME = /segfs/tango/cppapi

#LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib
#LIB_TANGO_DIR = $(TANGO_HOME)/../tmp/install/$(BIN_DIR)/lib


#
# Get BIN DIR
#

ifndef ubuntu
OS=$(shell /csadmin/common/scripts/get_os.share)
endif

ifdef ubuntu
OS=ubuntu1004
endif

BIN_DIR = $(OS)

ifdef _solaris
BIN_DIR = $(OS)_CC
endif

##############################################################################
#
# 		Define base directory
#
##############################################################################

ifdef prefix
TANGO_HOME = $(prefix)/$(BIN_DIR)
LIB_TANGO_DIR = $(TANGO_HOME)/lib
BIN_TANGO_DIR = $(TANGO_HOME)/bin
else
TANGO_HOME = /home/taurel/tango/cppapi
LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib
endif

##############################################################################
#
# 		OmniORB base directory
#
##############################################################################

OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/$(BIN_DIR)

##############################################################################
#
# 		Log4tango base directory
#
##############################################################################

LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/$(BIN_DIR)

ifdef _solaris
CC = /opt/SUNWspro/bin/CC
endif

ifdef linux
CC = c++
AR = ar
endif


##############################################################################
#
# 		Compilation
#
##############################################################################

ifdef prefix
INCLUDE_DIRS = -I$(OMNI_BASE)/include \
			   -I$(TANGO_HOME)/include \
			   -I$(LOG4TANGO_BASE)/include \
			   -I. 

LIB_DIRS = -L $(OMNI_BASE)/lib \
           -L $(LIB_TANGO_DIR)/debug \
	   	   -L $(LOG4TANGO_BASE)/lib
else
INCLUDE_DIRS = -I$(OMNI_BASE)/include \
			   -I$(TANGO_HOME)/server \
			   -I$(TANGO_HOME)/client \
			   -I$(LOG4TANGO_BASE)/include \
			   -I. 

LIB_DIRS = -L $(OMNI_BASE)/lib \
           -L $(LIB_TANGO_DIR) \
	   	   -L $(LOG4TANGO_BASE)/lib
endif


ifdef prefix
ifdef shared
LIB_TANGO = -ltango
else
LIB_TANGO = $(LIB_TANGO_DIR)/debug/libtango.a
endif
else
LIB_TANGO = -ltango
endif

ifdef _solaris
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LFLAGS =  -mt -g $(LIB_DIRS) $(LIB_TANGO) \
	-llog4tango -lomniORB4 -lomniDynamic4 -lomnithread -lpthread \
	-lCOS4 -lposix4 -lsocket -lnsl
endif

ifdef linux
CXXFLAGS =  -g -D_REENTRANT $(INCLUDE_DIRS)
LFLAGS =  -g $(LIB_DIRS) $(LIB_TANGO) \
	-llog4tango -lomniORB4  -lomniDynamic4 \
	-lCOS4 -lomnithread -lpthread -ldl 
endif


SVC_OBJS = 	main.o \
		TypeCmds.o \
		SigThrow.o \
		IOMisc.o \
		IOStr1.o \
		IOStr2.o \
		IOArray1.o \
		IOArray2.o \
		IOStrArray.o \
		IOStruct.o \
		IOSetAttr.o \
		IOAddAttribute.o \
		IOSeqVec.o \
		FileDb.o \
		classfactory.o \
		DevTestClass.o \
		DevTest.o



.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<


all: DevTest

DevTest:	$(SVC_OBJS)
	$(CC) $(SVC_OBJS) -o $(BIN_TANGO_DIR)/devTest $(LFLAGS) 

 
clean:
	rm -f *.o core






