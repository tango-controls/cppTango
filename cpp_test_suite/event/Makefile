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
OS=ubuntu1004
#OS=ubuntu904
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
TANGO_HOME = /home/taurel/tango/cppapi_develop
LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(BIN_DIR)/lib
BIN_TANGO_DIR = bin/$(BIN_DIR)
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

#LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.3/$(BIN_DIR)
LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango_trunk/$(BIN_DIR)


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
			   -I$(LOG4TANGO_BASE)/include/tango \
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
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread  \
	  -lCOS4 -lpthread -ldl
endif


#
# Rule
#

.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<


			
all: per_event change_event change_event64 archive_event user_event \
    server_event reco_event reco_svc state_event scan att_conf_event \
    data_ready_event \
    change_event_buffer att_conf_event_buffer data_ready_event_buffer \
	event_lock multi_event


per_event:	per_event.o
	@./cr_dir $(BIN_TANGO_DIR)
	$(CC) per_event.o -o $(BIN_TANGO_DIR)/per_event $(LFLAGS)

change_event:	change_event.o
	$(CC) change_event.o -o $(BIN_TANGO_DIR)/change_event $(LFLAGS)

change_event64:	change_event64.o
	$(CC) change_event64.o -o $(BIN_TANGO_DIR)/change_event64 $(LFLAGS)
	
change_event_buffer:	change_event_buffer.o
	$(CC) change_event_buffer.o -o $(BIN_TANGO_DIR)/change_event_buffer $(LFLAGS)

archive_event:	archive_event.o
	$(CC) archive_event.o -o $(BIN_TANGO_DIR)/archive_event $(LFLAGS)

user_event:	user_event.o
	$(CC) user_event.o -o $(BIN_TANGO_DIR)/user_event $(LFLAGS)

server_event:	server_event.o
	$(CC) server_event.o -o $(BIN_TANGO_DIR)/server_event $(LFLAGS)

multi_event:	multi_event.o
	$(CC) multi_event.o -o $(BIN_TANGO_DIR)/multi_event $(LFLAGS)

reco_event:	reco_event.o
	$(CC) reco_event.o -o $(BIN_TANGO_DIR)/reco_event $(LFLAGS)

reco_svc:	reco_svc.o
	$(CC) reco_svc.o -o $(BIN_TANGO_DIR)/reco_svc $(LFLAGS)
		
back_per_event:	back_per_event.o
	$(CC) back_per_event.o -o $(BIN_TANGO_DIR)/back_per_event $(LFLAGS)
	
back_ch_event:	back_ch_event.o
	$(CC) back_ch_event.o -o $(BIN_TANGO_DIR)/back_ch_event $(LFLAGS)

state_event:	state_event.o
	$(CC) state_event.o -o $(BIN_TANGO_DIR)/state_event $(LFLAGS)

scan:	scan.o
	$(CC) scan.o -o $(BIN_TANGO_DIR)/scan $(LFLAGS)
	
att_conf_event:	att_conf_event.o
	$(CC) att_conf_event.o -o $(BIN_TANGO_DIR)/att_conf_event $(LFLAGS)

att_conf_event_buffer: att_conf_event_buffer.o
	$(CC) att_conf_event_buffer.o -o $(BIN_TANGO_DIR)/att_conf_event_buffer $(LFLAGS)
	
data_ready_event:	data_ready_event.o
	$(CC) data_ready_event.o -o $(BIN_TANGO_DIR)/data_ready_event $(LFLAGS)
	
data_ready_event_buffer:	data_ready_event_buffer.o
	$(CC) data_ready_event_buffer.o -o $(BIN_TANGO_DIR)/data_ready_event_buffer $(LFLAGS)	

event_lock:	event_lock.o
	$(CC) event_lock.o -o $(BIN_TANGO_DIR)/event_lock $(LFLAGS)	
															
clean:
	rm -f *.o core
