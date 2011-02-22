#
#		Makefile to generate a Tango 
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

###################################################################


ifdef _solaris9
ifdef natif
CC = /opt/SUNWspro/bin/CC
BIN_DIR = solaris9_CC
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/solaris9_CC
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/solaris9_CC
else
CC = c++
BIN_DIR = solaris9_gcc
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/solaris9_gcc
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/solaris9_gcc
endif
endif

ifdef _solaris10
ifdef natif
CC = CC
BIN_DIR = solaris10_CC
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/solaris10_CC
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/solaris10_CC
endif
endif

ifdef linux
CC = c++
ifdef debian30
BIN_DIR = debian30
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/debian30
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/debian30
else
ifdef redhate4
BIN_DIR = redhate4
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate4
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate4
else
ifdef redhate4_64
BIN_DIR = redhate4_64
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate4_64
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate4_64
else
ifdef redhate5
BIN_DIR = redhate5
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate5
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate5
else
ifdef redhate5_64
BIN_DIR = redhate5_64
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/redhate5_64
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/redhate5_64
else
ifdef ubuntu904
BIN_DIR = ubuntu904
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.4/ubuntu904
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/ubuntu904
else
ifdef ubuntu1004
BIN_DIR = ubuntu1004
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


INCLUDE_DIRS = -I. \
	       -I$(TANGO_HOME)/client \
	       -I$(TANGO_HOME)/server \
	       -I$(OMNI_BASE)/include \
	       -I$(LOG4TANGO_BASE)/include
#INCLUDE_DIRS = -I. \
#		  -I/segfs/tango/release/ubuntu704/include \
#	      -I$(OMNI_BASE)/include \
#	      -I$(LOG4TANGO_BASE)/include

LIB_DIRS = -L $(LIB_TANGO_DIR) \
	   -L $(OMNI_BASE)/lib \
	   -L $(LOG4TANGO_BASE)/lib
#LIB_DIRS = -L /segfs/tango/release/ubuntu704/lib \
#	  -L $(OMNI_BASE)/lib \
#	  -L $(LOG4TANGO_BASE)/lib

ifdef _solaris9
ifdef natif
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g -mt $(LIB_DIRS) $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 \
	  -lposix4 -lsocket -lnsl -lpthread
else
CXXFLAGS = -g -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lposix4 -lsocket -lnsl -lpthread
endif
endif

ifdef _solaris10
ifdef natif
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g -mt $(LIB_DIRS) $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 \
	  -lposix4 -lsocket -lnsl -lpthread
endif
endif

ifdef linux
CXXFLAGS = -g -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
#CXXFLAGS = -g -DCOMPAT -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g $(LIB_DIRS)  -ltango -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread -lpthread -ldl
endif


.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<
	
				
all: asyn_cmd asyn_attr asyn_attr_multi asyn_write_attr asyn_write_attr_multi \
     asyn_cb asyn_cb2 asyn_cb_cmd asyn_attr_cb asyn_write_cb asyn_thread \
     auto_asyn_cmd
     
asyn_cmd:	asyn_cmd.o
	$(CC) asyn_cmd.o -o $(BIN_DIR)/asyn_cmd $(LFLAGS)

asyn_attr:	asyn_attr.o
	$(CC) asyn_attr.o -o $(BIN_DIR)/asyn_attr $(LFLAGS)

asyn_attr_multi:	asyn_attr_multi.o
	$(CC) asyn_attr_multi.o -o $(BIN_DIR)/asyn_attr_multi $(LFLAGS)

asyn_write_attr:	asyn_write_attr.o
	$(CC) asyn_write_attr.o -o $(BIN_DIR)/asyn_write_attr $(LFLAGS)
							
asyn_write_attr_multi:	asyn_write_attr_multi.o
	$(CC) asyn_write_attr_multi.o -o $(BIN_DIR)/asyn_write_attr_multi $(LFLAGS)

asyn_cb:	asyn_cb.o
	$(CC) asyn_cb.o -o $(BIN_DIR)/asyn_cb $(LFLAGS)

asyn_cb2:	asyn_cb2.o
	$(CC) asyn_cb2.o -o $(BIN_DIR)/asyn_cb2 $(LFLAGS)
	
asyn_cb_cmd:	asyn_cb_cmd.o
	$(CC) asyn_cb_cmd.o -o $(BIN_DIR)/asyn_cb_cmd $(LFLAGS)

asyn_attr_cb:	asyn_attr_cb.o
	$(CC) asyn_attr_cb.o -o $(BIN_DIR)/asyn_attr_cb $(LFLAGS)

asyn_write_cb:	asyn_write_cb.o
	$(CC) asyn_write_cb.o -o $(BIN_DIR)/asyn_write_cb $(LFLAGS)

asyn_thread:	asyn_thread.o
	$(CC) asyn_thread.o -o $(BIN_DIR)/asyn_thread $(LFLAGS)

auto_asyn_cmd:	auto_asyn_cmd.o
	$(CC) auto_asyn_cmd.o -o $(BIN_DIR)/auto_asyn_cmd $(LFLAGS)
	
asyn_faf:	asyn_faf.o
	$(CC) asyn_faf.o -o $(BIN_DIR)/asyn_faf $(LFLAGS)
						
clean:
	rm -f *.o core
