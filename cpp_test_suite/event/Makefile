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
#	      -I/segfs/tango/release/ubuntu904/include \
#	      -I$(OMNI_BASE)/include \
#	      -I$(LOG4TANGO_BASE)/include

LIB_DIRS = -L $(LIB_TANGO_DIR) \
	   -L $(OMNI_BASE)/lib \
	   -L $(LOG4TANGO_BASE)/lib
#LIB_DIRS = -L /segfs/tango/release/ubuntu904/lib \
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
	$(CC) per_event.o -o $(BIN_DIR)/per_event $(LFLAGS)

change_event:	change_event.o
	$(CC) change_event.o -o $(BIN_DIR)/change_event $(LFLAGS)

change_event64:	change_event64.o
	$(CC) change_event64.o -o $(BIN_DIR)/change_event64 $(LFLAGS)
	
change_event_buffer:	change_event_buffer.o
	$(CC) change_event_buffer.o -o $(BIN_DIR)/change_event_buffer $(LFLAGS)

archive_event:	archive_event.o
	$(CC) archive_event.o -o $(BIN_DIR)/archive_event $(LFLAGS)

user_event:	user_event.o
	$(CC) user_event.o -o $(BIN_DIR)/user_event $(LFLAGS)

server_event:	server_event.o
	$(CC) server_event.o -o $(BIN_DIR)/server_event $(LFLAGS)

multi_event:	multi_event.o
	$(CC) multi_event.o -o $(BIN_DIR)/multi_event $(LFLAGS)

reco_event:	reco_event.o
	$(CC) reco_event.o -o $(BIN_DIR)/reco_event $(LFLAGS)

reco_svc:	reco_svc.o
	$(CC) reco_svc.o -o $(BIN_DIR)/reco_svc $(LFLAGS)
		
back_per_event:	back_per_event.o
	$(CC) back_per_event.o -o $(BIN_DIR)/back_per_event $(LFLAGS)
	
back_ch_event:	back_ch_event.o
	$(CC) back_ch_event.o -o $(BIN_DIR)/back_ch_event $(LFLAGS)

state_event:	state_event.o
	$(CC) state_event.o -o $(BIN_DIR)/state_event $(LFLAGS)

scan:	scan.o
	$(CC) scan.o -o $(BIN_DIR)/scan $(LFLAGS)
	
att_conf_event:	att_conf_event.o
	$(CC) att_conf_event.o -o $(BIN_DIR)/att_conf_event $(LFLAGS)

att_conf_event_buffer: att_conf_event_buffer.o
	$(CC) att_conf_event_buffer.o -o $(BIN_DIR)/att_conf_event_buffer $(LFLAGS)
	
data_ready_event:	data_ready_event.o
	$(CC) data_ready_event.o -o $(BIN_DIR)/data_ready_event $(LFLAGS)
	
data_ready_event_buffer:	data_ready_event_buffer.o
	$(CC) data_ready_event_buffer.o -o $(BIN_DIR)/data_ready_event_buffer $(LFLAGS)	

event_lock:	event_lock.o
	$(CC) event_lock.o -o $(BIN_DIR)/event_lock $(LFLAGS)	
															
clean:
	rm -f *.o core
