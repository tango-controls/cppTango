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
BIN_DIR_SL = suse82_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/suse82
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/suse82
else
ifdef debian30
BIN_DIR = debian30
BIN_DIR_SL = debian30_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/debian30
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/debian30
else
ifdef suse72_gcc32
BIN_DIR = suse72_gcc32
BIN_DIR_SL = suse72_gcc32_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/suse72_gcc32
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/suse72_gcc32
else
ifdef suse90_64
BIN_DIR = suse90_64
BIN_DIR_SL = suse90_64_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/suse90_64
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/suse90_64
else
ifdef redhat40
BIN_DIR = redhat40
BIN_DIR_SL = redhat40_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/redhat40
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/redhat40
else
BIN_DIR = suse72
BIN_DIR_SL = suse72_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/suse72
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.0/suse72
endif
endif
endif
endif
endif
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
	  -lomniORB4 -lomniDynamic4 -lomnithread  \
	  -lCOS4 -lsocket -lnsl -lpthread -lposix4
endif

ifdef _solaris9_gcc
CXXFLAGS = -g -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/$(BIN_DIR)/libtango.a
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread  \
	  -lCOS4 -lsocket -lnsl -lpthread -lposix4
endif

ifdef linux
CXXFLAGS = -g -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/$(BIN_DIR)/libtango.a
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


			
all: per_event change_event quality_event archive_event user_event \
     server_event reco_event reco_svc state_event scan


per_event:	per_event.o
	$(CC) per_event.o -o $(BIN_DIR)/per_event $(LFLAGS)

change_event:	change_event.o
	$(CC) change_event.o -o $(BIN_DIR)/change_event $(LFLAGS)

quality_event:	quality_event.o
	$(CC) quality_event.o -o $(BIN_DIR)/quality_event $(LFLAGS)

archive_event:	archive_event.o
	$(CC) archive_event.o -o $(BIN_DIR)/archive_event $(LFLAGS)

user_event:	user_event.o
	$(CC) user_event.o -o $(BIN_DIR)/user_event $(LFLAGS)

server_event:	server_event.o
	$(CC) server_event.o -o $(BIN_DIR)/server_event $(LFLAGS)

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
															
clean:
	rm -f *.o core
