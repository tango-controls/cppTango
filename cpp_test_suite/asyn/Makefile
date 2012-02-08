#############################################################
#
#		Makefile to generate a Tango test suite
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
BIN_TANGO_DIR = $(TANGO_HOME)/bin
else
TANGO_HOME = /home/taurel/tango/cppapi_develop
LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib
BIN_TANGO_DIR = bin/$(BIN_DIR)
endif

##############################################################################
#
# 		OmniORB base directory
#
##############################################################################

ifdef prefix
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.6/$(BIN_DIR)
else
OMNI_BASE = /segfs/tango/ORB/omniORB4.1.6/$(BIN_DIR)
endif

##############################################################################
#
# 		Log4tango base directory
#
##############################################################################

ifdef prefix
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.5/$(BIN_DIR)
else
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.5/$(BIN_DIR)
endif

##############################################################################
#
# 		ZMQ base directory
#
##############################################################################

ifdef prefix
ZMQ_BASE = /segfs/tango/transport/zmq/$(BIN_DIR)
else
ZMQ_BASE = /segfs/tango/transport/zmq/$(BIN_DIR)
endif

ifdef linux
CC = c++
endif

##############################################################################
#
# 		Compilation
#
##############################################################################

OBJS_DIR = 	objs

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

ifdef linux
#Check that we have at least gcc 4.3 (for c++0x features)
GCC_MAJOR_VERSION_GTEQ4 := $(shell expr `c++ -dumpversion | cut -f1 -d.` \>= 4)
GCC_MINOR_VERSION_GTEQ3 := $(shell expr `c++ -dumpversion | cut -f2 -d.` \>= 3)
ifeq ($(GCC_MAJOR_VERSION_GTEQ4),1)
    ifeq ($(GCC_MINOR_VERSION_GTEQ3),1)
        CXX11 = -std=c++0x
    endif
endif

CXXFLAGS = -g $(CXX11) -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g $(LIB_DIRS)  -ltango -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lCOS4 -lzmq -lomnithread -lpthread -ldl
endif


#.SUFFIXES:	.o .cpp
#.cpp.o:
#	$(CC) $(CXXFLAGS) -c $<
	
$(OBJS_DIR)/%.o: %.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o
				
all: asyn_cmd asyn_attr asyn_attr_multi asyn_write_attr asyn_write_attr_multi \
     asyn_cb asyn_cb2 asyn_cb_cmd asyn_attr_cb asyn_write_cb asyn_thread \
     auto_asyn_cmd
     
asyn_cmd:	$(OBJS_DIR)/asyn_cmd.o
	@./cr_dir $(BIN_TANGO_DIR)
	$(CC) $(OBJS_DIR)/asyn_cmd.o -o $(BIN_TANGO_DIR)/asyn_cmd $(LFLAGS)

asyn_attr:	$(OBJS_DIR)/asyn_attr.o
	$(CC) $(OBJS_DIR)/asyn_attr.o -o $(BIN_TANGO_DIR)/asyn_attr $(LFLAGS)

asyn_attr_multi:	$(OBJS_DIR)/asyn_attr_multi.o
	$(CC) $(OBJS_DIR)/asyn_attr_multi.o -o $(BIN_TANGO_DIR)/asyn_attr_multi $(LFLAGS)

asyn_write_attr:	$(OBJS_DIR)/asyn_write_attr.o
	$(CC) $(OBJS_DIR)/asyn_write_attr.o -o $(BIN_TANGO_DIR)/asyn_write_attr $(LFLAGS)
							
asyn_write_attr_multi:	$(OBJS_DIR)/asyn_write_attr_multi.o
	$(CC) $(OBJS_DIR)/asyn_write_attr_multi.o -o $(BIN_TANGO_DIR)/asyn_write_attr_multi $(LFLAGS)

asyn_cb:	$(OBJS_DIR)/asyn_cb.o
	$(CC) $(OBJS_DIR)/asyn_cb.o -o $(BIN_TANGO_DIR)/asyn_cb $(LFLAGS)

asyn_cb2:	$(OBJS_DIR)/asyn_cb2.o
	$(CC) $(OBJS_DIR)/asyn_cb2.o -o $(BIN_TANGO_DIR)/asyn_cb2 $(LFLAGS)
	
asyn_cb_cmd:	$(OBJS_DIR)/asyn_cb_cmd.o
	$(CC) $(OBJS_DIR)/asyn_cb_cmd.o -o $(BIN_TANGO_DIR)/asyn_cb_cmd $(LFLAGS)

asyn_attr_cb:	$(OBJS_DIR)/asyn_attr_cb.o
	$(CC) $(OBJS_DIR)/asyn_attr_cb.o -o $(BIN_TANGO_DIR)/asyn_attr_cb $(LFLAGS)

asyn_write_cb:	$(OBJS_DIR)/asyn_write_cb.o
	$(CC) $(OBJS_DIR)/asyn_write_cb.o -o $(BIN_TANGO_DIR)/asyn_write_cb $(LFLAGS)

asyn_thread:	$(OBJS_DIR)/asyn_thread.o
	$(CC) $(OBJS_DIR)/asyn_thread.o -o $(BIN_TANGO_DIR)/asyn_thread $(LFLAGS)

auto_asyn_cmd:	$(OBJS_DIR)/auto_asyn_cmd.o
	$(CC) $(OBJS_DIR)/auto_asyn_cmd.o -o $(BIN_TANGO_DIR)/auto_asyn_cmd $(LFLAGS)
	
asyn_faf:	$(OBJS_DIR)/asyn_faf.o
	$(CC) $(OBJS_DIR)/asyn_faf.o -o $(BIN_TANGO_DIR)/asyn_faf $(LFLAGS)
						
clean:
	rm -f $(OBJS_DIR)/*.o core
