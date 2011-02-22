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
TANGO_HOME = /segfs/tango/cppapi

#LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib
LIB_TANGO_DIR = $(TANGO_HOME)/../tmp/install/$(BIN_DIR)/lib

ifdef _solaris9
ifdef natif
CC = /opt/SUNWspro/bin/CC
DEST_BIN_DIR = bin/solaris9_CC
BIN_DIR = solaris9_CC
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/solaris9_CC
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/solaris9_CC
else
CC = c++
DEST_BIN_DIR = bin/solaris9_gcc
BIN_DIR = solaris9_gcc
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/solaris9_gcc
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/solaris9_gcc
endif
endif

ifdef _solaris10
ifdef natif
CC = /opt/SUNWspro/bin/CC
DEST_BIN_DIR = bin/solaris10_CC
BIN_DIR = solaris10_CC
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/solaris10_CC
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/solaris10_CC
endif
endif

ifdef linux
CC = c++
AR = ar
ifdef debian30
DEST_BIN_DIR = bin/debian30
BIN_DIR =  debian30
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/debian30
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/debian30
else
ifdef redhate4
DEST_BIN_DIR = bin/redhate4
BIN_DIR =  redhate4
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate4
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate4
else
ifdef redhate4_64
DEST_BIN_DIR = bin/redhate4_64
BIN_DIR =  redhate4_64
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate4_64
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate4_64
else
ifdef redhate5
DEST_BIN_DIR = bin/redhate5
BIN_DIR =  redhate5
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate5
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate5
else
ifdef redhate5_64
DEST_BIN_DIR = bin/redhate5_64
BIN_DIR =  redhate5_64
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate5_64
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate5_64
else
ifdef ubuntu904
DEST_BIN_DIR = bin/ubuntu904
BIN_DIR =  ubuntu904
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/ubuntu904
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/ubuntu904
else
ifdef ubuntu1004
DEST_BIN_DIR = bin/ubuntu1004
BIN_DIR =  ubuntu1004
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/ubuntu1004
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/ubuntu1004
endif
endif
endif
endif
endif
endif
endif
endif


INCLUDE_DIRS = -I$(OMNI_BASE)/include \
		-I$(TANGO_HOME)/server \
		-I$(TANGO_HOME)/client \
		-I$(LOG4TANGO_BASE)/include \
		-I. 
#INCLUDE_DIRS = -I. \
#		  -I/segfs/tango/release/ubuntu904/include \
#	      -I$(OMNI_BASE)/include \
#	      -I$(LOG4TANGO_BASE)/include
	       	       
LIB_DIRS = -L $(OMNI_BASE)/lib \
           -L $(LIB_TANGO_DIR) \
	   -L $(LOG4TANGO_BASE)/lib
#LIB_DIRS = -L /segfs/tango/release/ubuntu904/lib \
#	  -L $(OMNI_BASE)/lib \
#	  -L $(LOG4TANGO_BASE)/lib


ifdef _solaris9
ifdef natif
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LFLAGS =  -mt -g $(LIB_DIRS) -ltango \
	-llog4tango -lomniORB4 -lomniDynamic4 -lomnithread -lpthread \
	-lCOS4 -lposix4 -lsocket -lnsl
else
CXXFLAGS =  -g -D_REENTRANT $(INCLUDE_DIRS)
LFLAGS =  $(LIB_DIRS) -ltango \
	-llog4tango -lomniORB4  -lomniDynamic4 \
	-lCOS4 -lomnithread -lposix4 -lsocket -lnsl
endif
endif

ifdef _solaris10
ifdef natif
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LFLAGS =  -mt -g $(LIB_DIRS) -ltango \
	-llog4tango -lomniORB4 -lomniDynamic4 -lomnithread -lpthread \
	-lCOS4 -lposix4 -lsocket -lnsl
endif
endif

ifdef linux
CXXFLAGS =  -g -D_REENTRANT $(INCLUDE_DIRS)
#CXXFLAGS =  -g -DCOMPAT -D_REENTRANT $(INCLUDE_DIRS)
LFLAGS =  -g $(LIB_DIRS) -ltango \
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
	$(CC) $(SVC_OBJS) -o $(DEST_BIN_DIR)/devTest $(LFLAGS) 

 
clean:
	rm -f *.o core






