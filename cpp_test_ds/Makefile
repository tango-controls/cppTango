##############################################################
#
#		Makefile to generate the Tango test server
#
##############################################################

#
# Get BIN DIR
#

ifndef ubuntu
OS=$(shell /csadmin/common/scripts/get_os.share)
endif

ifdef ubuntu
ifdef dev
OS=ubuntu1010
else
OS=ubuntu1004
endif
endif

BIN_DIR = $(OS)


##############################################################################
#
# 		Define base directory
#
##############################################################################

ifdef prefix
TANGO_HOME = $(prefix)/$(BIN_DIR)
LIB_TANGO_DIR = $(TANGO_HOME)/lib
ifdef shared
BIN_TANGO_DIR = $(TANGO_HOME)/bin/shared
else
BIN_TANGO_DIR = $(TANGO_HOME)/bin/archive
endif
else
TANGO_HOME = /home/taurel/tango/cppapi_develop
LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib/debug
BIN_TANGO_DIR = bin/$(BIN_DIR)
endif

##############################################################################
#
# 		OmniORB base directory
#
##############################################################################

OMNI_BASE = /segfs/tango/ORB/omniORB4.1.6/$(BIN_DIR)

##############################################################################
#
# 		Log4tango base directory
#
##############################################################################

LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.4/$(BIN_DIR)

##############################################################################
#
# 		ZMQ base directory
#
##############################################################################

#ZMQ_BASE = /home/taurel/bin/zmq_pgm
ZMQ_BASE = /segfs/tango/transport/zmq/$(BIN_DIR)

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
			   -I$(LOG4TANGO_BASE)/include/tango \
			   -I$(ZMQ_BASE)/include \
			   -I.

LIB_DIRS = -L $(OMNI_BASE)/lib \
           -L $(LIB_TANGO_DIR)/debug \
	   	   -L $(LOG4TANGO_BASE)/lib \
		   -L $(ZMQ_BASE)/lib
else
INCLUDE_DIRS = -I$(OMNI_BASE)/include \
		-I$(TANGO_HOME)/server \
		-I$(TANGO_HOME)/client \
		-I$(LOG4TANGO_BASE)/include/tango \
		-I$(ZMQ_BASE)/include \
		-I.

LIB_DIRS = -L $(OMNI_BASE)/lib \
           -L $(LIB_TANGO_DIR) \
	   -L $(LOG4TANGO_BASE)/lib \
	   -L $(ZMQ_BASE)/lib
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


ifdef linux

#Check that we have at least gcc 4.3 (for c++0x features)

GCC_MAJOR_VERSION_GTEQ4 := $(shell expr `c++ -dumpversion | cut -f1 -d.` \>= 4)
GCC_MINOR_VERSION_GTEQ3 := $(shell expr `c++ -dumpversion | cut -f2 -d.` \>= 3)
ifeq ($(GCC_MAJOR_VERSION_GTEQ4),1)
    ifeq ($(GCC_MINOR_VERSION_GTEQ3),1)
        CXX11 = -std=c++0x
    endif
endif

CXXFLAGS =  -g $(CXX11) -D_REENTRANT $(INCLUDE_DIRS)
LFLAGS =  -g $(LIB_DIRS) $(LIB_TANGO) \
	-llog4tango -lomniORB4  -lomniDynamic4 \
	-lCOS4 -lomnithread -lzmq -lpthread -ldl
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
	@./cr_dir $(BIN_TANGO_DIR)
	$(CC) $(SVC_OBJS) -o $(BIN_TANGO_DIR)/devTest $(LFLAGS)


clean:
	rm -f *.o core






