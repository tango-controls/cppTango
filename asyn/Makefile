#
#		Makefile to generate a Tango 
#

TANGO_BASE = /segfs/tango/cppapi

ifdef _solaris
CC = CC
BIN_DIR = solaris7_CC
BIN_DIR_SL = solaris7_CC_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/solaris7_CC
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/solaris7_CC
endif

ifdef _solaris9
CC = CC
BIN_DIR = solaris9_CC
BIN_DIR_SL = solaris9_CC_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/solaris9_CC
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/solaris9_CC
endif

ifdef _solaris7_gcc
CC = c++
BIN_DIR = solaris7
BIN_DIR_SL = solaris7_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/solaris7
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/solaris7
endif

ifdef _solaris9_gcc
CC = c++
BIN_DIR = solaris9
BIN_DIR_SL = solaris9_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/solaris9
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/solaris9
endif


ifdef linux
CC = c++
ifdef suse82
BIN_DIR = suse82
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/$(BIN_DIR)
else
ifdef debian30
BIN_DIR = debian30
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/$(BIN_DIR)
else
ifdef suse72_gcc32
BIN_DIR = suse72_gcc32
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/$(BIN_DIR)
else
ifdef redhat40
BIN_DIR = redhat40
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/$(BIN_DIR)
else
ifdef suse90_64
BIN_DIR = suse90_64
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/$(BIN_DIR)
else
BIN_DIR = suse72
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.0/$(BIN_DIR)
endif
endif
endif
endif
endif
BIN_DIR_SL = $(BIN_DIR)_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/$(BIN_DIR)
endif


TANGO_SVC_HOME = /segfs/tango/include

INCLUDE_DIRS = -I. \
	       -I$(TANGO_BASE)/client \
	       -I$(TANGO_BASE)/server \
	       -I$(OMNI_BASE)/include \
	       -I$(LOG4TANGO_BASE)/include

LIB_DIRS = -L $(TANGO_BASE)/server/$(BIN_DIR) \
	   -L $(OMNI_BASE)/lib \
	   -L $(LOG4TANGO_BASE)/lib

ifdef _solaris
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/solaris7_CC/libtango.a
LFLAGS =  $(LIB_DIRS) -g -mt $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 \
	  -lposix4 -lsocket -lnsl -lpthread
endif

ifdef _solaris9
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/solaris9_CC/libtango.a
LFLAGS =  $(LIB_DIRS) -g -mt $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 \
	  -lposix4 -lsocket -lnsl -lpthread
endif

ifdef _solaris7_gcc
CXXFLAGS = -g -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/$(BIN_DIR)/libtango.a
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread \
	  -lposix4 -lsocket -lnsl -lpthread
endif

ifdef _solaris9_gcc
CXXFLAGS = -g -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/$(BIN_DIR)/libtango.a
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread \
	  -lposix4 -lsocket -lnsl -lpthread
endif

ifdef linux
CXXFLAGS = -g -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/$(BIN_DIR)/libtango.a
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread -lpthread -ldl
endif


.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<
	
				
all: asyn_cmd asyn_attr asyn_attr_multi asyn_write_attr asyn_write_attr_multi \
     asyn_cb asyn_cb2 asyn_cb_cmd asyn_attr_cb asyn_write_cb asyn_thread \
     auto_asyn_cmd
#all: asyn_cmd asyn_attr asyn_write_attr  \
#     asyn_cb asyn_cb2 asyn_cb_cmd asyn_attr_cb  asyn_thread \
#     auto_asyn_cmd
     
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
