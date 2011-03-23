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
#TANGO_HOME = /segfs/tango/cppapi

#LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib
#LIB_TANGO_DIR = $(TANGO_HOME)/../tmp/install/$(BIN_DIR)/lib

###################################################################

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


ifdef _solaris
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g -mt $(LIB_DIRS) $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 \
	  -lposix4 -lsocket -lnsl -lpthread
endif


ifdef linux
CXXFLAGS = -g -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
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
	@./cr_dir $(BIN_TANGO_DIR)
	$(CC) asyn_cmd.o -o $(BIN_TANGO_DIR)/asyn_cmd $(LFLAGS)

asyn_attr:	asyn_attr.o
	$(CC) asyn_attr.o -o $(BIN_TANGO_DIR)/asyn_attr $(LFLAGS)

asyn_attr_multi:	asyn_attr_multi.o
	$(CC) asyn_attr_multi.o -o $(BIN_TANGO_DIR)/asyn_attr_multi $(LFLAGS)

asyn_write_attr:	asyn_write_attr.o
	$(CC) asyn_write_attr.o -o $(BIN_TANGO_DIR)/asyn_write_attr $(LFLAGS)
							
asyn_write_attr_multi:	asyn_write_attr_multi.o
	$(CC) asyn_write_attr_multi.o -o $(BIN_TANGO_DIR)/asyn_write_attr_multi $(LFLAGS)

asyn_cb:	asyn_cb.o
	$(CC) asyn_cb.o -o $(BIN_TANGO_DIR)/asyn_cb $(LFLAGS)

asyn_cb2:	asyn_cb2.o
	$(CC) asyn_cb2.o -o $(BIN_TANGO_DIR)/asyn_cb2 $(LFLAGS)
	
asyn_cb_cmd:	asyn_cb_cmd.o
	$(CC) asyn_cb_cmd.o -o $(BIN_TANGO_DIR)/asyn_cb_cmd $(LFLAGS)

asyn_attr_cb:	asyn_attr_cb.o
	$(CC) asyn_attr_cb.o -o $(BIN_TANGO_DIR)/asyn_attr_cb $(LFLAGS)

asyn_write_cb:	asyn_write_cb.o
	$(CC) asyn_write_cb.o -o $(BIN_TANGO_DIR)/asyn_write_cb $(LFLAGS)

asyn_thread:	asyn_thread.o
	$(CC) asyn_thread.o -o $(BIN_TANGO_DIR)/asyn_thread $(LFLAGS)

auto_asyn_cmd:	auto_asyn_cmd.o
	$(CC) auto_asyn_cmd.o -o $(BIN_TANGO_DIR)/auto_asyn_cmd $(LFLAGS)
	
asyn_faf:	asyn_faf.o
	$(CC) asyn_faf.o -o $(BIN_TANGO_DIR)/asyn_faf $(LFLAGS)
						
clean:
	rm -f *.o core
