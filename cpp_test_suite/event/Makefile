##############################################################
#
#		Makefile to generate a Tango 
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
OS=ubuntu1204
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
LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib/debug
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
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.6/$(BIN_DIR)
else
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.6/$(BIN_DIR)
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
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread  \
	  -lCOS4 -lzmq -lpthread -ldl
endif


#
# Rule
#

#.SUFFIXES:	.o .cpp
#.cpp.o:
#	$(CC) $(CXXFLAGS) -c $<

$(OBJS_DIR)/%.o: %.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o

			
all: per_event change_event change_event64 archive_event user_event \
    server_event reco_event reco_svc state_event scan att_conf_event \
    data_ready_event \
    change_event_buffer att_conf_event_buffer data_ready_event_buffer \
	event_lock multi_event att_type_event stateless_sub reco_zmq


per_event:	$(OBJS_DIR)/per_event.o
	@./cr_dir $(BIN_TANGO_DIR)
	$(CC) $(OBJS_DIR)/per_event.o -o $(BIN_TANGO_DIR)/per_event $(LFLAGS)

change_event:	$(OBJS_DIR)/change_event.o
	$(CC) $(OBJS_DIR)/change_event.o -o $(BIN_TANGO_DIR)/change_event $(LFLAGS)

change_event64:	$(OBJS_DIR)/change_event64.o
	$(CC) $(OBJS_DIR)/change_event64.o -o $(BIN_TANGO_DIR)/change_event64 $(LFLAGS)
	
change_event_buffer:	$(OBJS_DIR)/change_event_buffer.o
	$(CC) $(OBJS_DIR)/change_event_buffer.o -o $(BIN_TANGO_DIR)/change_event_buffer $(LFLAGS)

archive_event:	$(OBJS_DIR)/archive_event.o
	$(CC) $(OBJS_DIR)/archive_event.o -o $(BIN_TANGO_DIR)/archive_event $(LFLAGS)

user_event:	$(OBJS_DIR)/user_event.o
	$(CC) $(OBJS_DIR)/user_event.o -o $(BIN_TANGO_DIR)/user_event $(LFLAGS)

server_event:	$(OBJS_DIR)/server_event.o
	$(CC) $(OBJS_DIR)/server_event.o -o $(BIN_TANGO_DIR)/server_event $(LFLAGS)

multi_event:	$(OBJS_DIR)/multi_event.o
	$(CC) $(OBJS_DIR)/multi_event.o -o $(BIN_TANGO_DIR)/multi_event $(LFLAGS)

reco_event:	$(OBJS_DIR)/reco_event.o
	$(CC) $(OBJS_DIR)/reco_event.o -o $(BIN_TANGO_DIR)/reco_event $(LFLAGS)

reco_svc:	$(OBJS_DIR)/reco_svc.o
	$(CC) $(OBJS_DIR)/reco_svc.o -o $(BIN_TANGO_DIR)/reco_svc $(LFLAGS)
		
back_per_event:	$(OBJS_DIR)/back_per_event.o
	$(CC) $(OBJS_DIR)/back_per_event.o -o $(BIN_TANGO_DIR)/back_per_event $(LFLAGS)
	
back_ch_event:	$(OBJS_DIR)/back_ch_event.o
	$(CC) $(OBJS_DIR)/back_ch_event.o -o $(BIN_TANGO_DIR)/back_ch_event $(LFLAGS)

state_event:	$(OBJS_DIR)/state_event.o
	$(CC) $(OBJS_DIR)/state_event.o -o $(BIN_TANGO_DIR)/state_event $(LFLAGS)

scan:	$(OBJS_DIR)/scan.o
	$(CC) $(OBJS_DIR)/scan.o -o $(BIN_TANGO_DIR)/scan $(LFLAGS)
	
att_conf_event:	$(OBJS_DIR)/att_conf_event.o
	$(CC) $(OBJS_DIR)/att_conf_event.o -o $(BIN_TANGO_DIR)/att_conf_event $(LFLAGS)

att_conf_event_buffer: $(OBJS_DIR)/att_conf_event_buffer.o
	$(CC) $(OBJS_DIR)/att_conf_event_buffer.o -o $(BIN_TANGO_DIR)/att_conf_event_buffer $(LFLAGS)
	
data_ready_event:	$(OBJS_DIR)/data_ready_event.o
	$(CC) $(OBJS_DIR)/data_ready_event.o -o $(BIN_TANGO_DIR)/data_ready_event $(LFLAGS)
	
data_ready_event_buffer:	$(OBJS_DIR)/data_ready_event_buffer.o
	$(CC) $(OBJS_DIR)/data_ready_event_buffer.o -o $(BIN_TANGO_DIR)/data_ready_event_buffer $(LFLAGS)	

event_lock:	$(OBJS_DIR)/event_lock.o
	$(CC) $(OBJS_DIR)/event_lock.o -o $(BIN_TANGO_DIR)/event_lock $(LFLAGS)

stateless_sub:	$(OBJS_DIR)/stateless_sub.o
	$(CC) $(OBJS_DIR)/stateless_sub.o -o $(BIN_TANGO_DIR)/stateless_sub $(LFLAGS)

reco_zmq:	$(OBJS_DIR)/reco_zmq.o
	$(CC) $(OBJS_DIR)/reco_zmq.o -o $(BIN_TANGO_DIR)/reco_zmq $(LFLAGS)	
					
att_type_event:	$(OBJS_DIR)/att_type_event.o
	$(CC) $(OBJS_DIR)/att_type_event.o -o $(BIN_TANGO_DIR)/att_type_event $(LFLAGS)										
	
clean:
	rm -f $(OBJS_DIR)/*.o core
