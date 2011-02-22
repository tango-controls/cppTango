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
#BIN_DIR_SL = suse72_gcc32_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/suse72_gcc32
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/suse72_gcc32
else
ifdef redhat40
BIN_DIR = redhat40
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/redhat40
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/redhat40
else
ifdef suse90_64
BIN_DIR = suse90_64
BIN_DIR_SL = suse90_64_sl
OMNI_BASE = /segfs/tango/ORB/omniORB4.0.5/suse90_64
LOG4TANGO_BASE = /segfs/tango/tools/log4tango4.0.2/suse90_64
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
LFLAGS =  -g -mt $(LIB_DIRS) $(LIB_TANGO) -llog4tango \
	 -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 \
	 -lposix4 -lsocket -lnsl -lpthread
endif

ifdef _solaris9
CXXFLAGS =  -g -mt $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/solaris9_CC/libtango.a
LFLAGS =  -g -mt $(LIB_DIRS) $(LIB_TANGO) -llog4tango \
	  -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 \
	  -lposix4 -lsocket -lnsl -lpthread
endif

ifdef _solaris7_gcc
CXXFLAGS = -g -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/solaris7/libtango.a
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lposix4 -lsocket -lnsl -lpthread
endif

ifdef _solaris9_gcc
CXXFLAGS = -g -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = $(TANGO_BASE)/server/solaris9/libtango.a
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lposix4 -lsocket -lnsl -lpthread
endif

ifdef linux
CXXFLAGS = -g -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
ifdef suse82
LIB_TANGO = $(TANGO_BASE)/server/suse82/libtango.a
else
ifdef debian30
LIB_TANGO = $(TANGO_BASE)/server/debian30/libtango.a
else
ifdef suse72_gcc32
LIB_TANGO = $(TANGO_BASE)/server/suse72_gcc32/libtango.a
else
ifdef redhat40
LIB_TANGO = $(TANGO_BASE)/server/redhat40/libtango.a
else
ifdef suse90_64
LIB_TANGO = $(TANGO_BASE)/server/suse90_64/libtango.a
else
LIB_TANGO = $(TANGO_BASE)/server/suse72/libtango.a
endif
endif
endif
endif
endif
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lpthread -ldl
endif

.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<
	
	
				
all: new_devproxy multiple_new reconnect cmd_inout read_attr cmd_types \
     attr_types misc_devdata misc_devattr misc_devproxy write_attr \
     copy_devproxy obj_prop read_hist poll_met poll_except print_data \
     print_data_hist syntax size attr_proxy read_hist_ext write_attr_3 \
     ring_depth attr_misc state_attr rds mem_att 64 attr_manip
# helper
    

	
new_devproxy:	new_devproxy.o
	$(CC) new_devproxy.o -o $(BIN_DIR)/new_devproxy $(LFLAGS)

multiple_new:	multiple_new.o
	$(CC) multiple_new.o -o $(BIN_DIR)/multiple_new $(LFLAGS)

reconnect:	reconnect.o
	$(CC) reconnect.o -o $(BIN_DIR)/reconnect $(LFLAGS)

reconnect_attr:	reconnect_attr.o
	$(CC) reconnect_attr.o -o $(BIN_DIR)/reconnect_attr $(LFLAGS)
	
cmd_inout:	cmd_inout.o
	$(CC) cmd_inout.o -o $(BIN_DIR)/cmd_inout $(LFLAGS)

read_attr:	read_attr.o
	$(CC) read_attr.o -o $(BIN_DIR)/read_attr $(LFLAGS)

cmd_types:	cmd_types.o
	$(CC) cmd_types.o -o $(BIN_DIR)/cmd_types $(LFLAGS)

attr_types:	attr_types.o
	$(CC) attr_types.o -o $(BIN_DIR)/attr_types $(LFLAGS)

misc_devdata:	misc_devdata.o
	$(CC) misc_devdata.o -o $(BIN_DIR)/misc_devdata $(LFLAGS)
		
misc_devattr:	misc_devattr.o
	$(CC) misc_devattr.o -o $(BIN_DIR)/misc_devattr $(LFLAGS)
						
misc_devproxy:	misc_devproxy.o
	$(CC) misc_devproxy.o -o $(BIN_DIR)/misc_devproxy $(LFLAGS)

write_attr:	write_attr.o
	$(CC) write_attr.o -o $(BIN_DIR)/write_attr $(LFLAGS)
		
copy_devproxy:	copy_devproxy.o
	$(CC) copy_devproxy.o -o $(BIN_DIR)/copy_devproxy $(LFLAGS)

obj_prop:	obj_prop.o
	$(CC) obj_prop.o -o $(BIN_DIR)/obj_prop $(LFLAGS)

read_hist:	read_hist.o
	$(CC) read_hist.o -o $(BIN_DIR)/read_hist $(LFLAGS)

poll_met:	poll_met.o
	$(CC) poll_met.o -o $(BIN_DIR)/poll_met $(LFLAGS)

poll_except:	poll_except.o
	$(CC) poll_except.o -o $(BIN_DIR)/poll_except $(LFLAGS)

print_data:	print_data.o
	$(CC) print_data.o -o $(BIN_DIR)/print_data $(LFLAGS)

print_data_hist:	print_data_hist.o
	$(CC) print_data_hist.o -o $(BIN_DIR)/print_data_hist $(LFLAGS)

syntax:	syntax.o
	$(CC) syntax.o -o $(BIN_DIR)/syntax $(LFLAGS)
						
size:	size.o
	$(CC) size.o -o $(BIN_DIR)/size $(LFLAGS)

write_attr_3:	write_attr_3.o
	$(CC) write_attr_3.o -o $(BIN_DIR)/write_attr_3 $(LFLAGS)

read_hist_ext:	read_hist_ext.o
	$(CC) read_hist_ext.o -o $(BIN_DIR)/read_hist_ext $(LFLAGS)

attr_proxy:	attr_proxy.o
	$(CC) attr_proxy.o -o $(BIN_DIR)/attr_proxy $(LFLAGS)
				
ring_depth:	ring_depth.o
	$(CC) ring_depth.o -o $(BIN_DIR)/ring_depth $(LFLAGS)

attr_misc:	attr_misc.o
	$(CC) attr_misc.o -o $(BIN_DIR)/attr_misc $(LFLAGS)

state_attr:	state_attr.o
	$(CC) state_attr.o -o $(BIN_DIR)/state_attr $(LFLAGS)

rds:	rds.o
	$(CC) rds.o -o $(BIN_DIR)/rds $(LFLAGS)

mem_att:	mem_att.o
	$(CC) mem_att.o -o $(BIN_DIR)/mem_att $(LFLAGS)

helper:	helper.o
	$(CC) helper.o -o $(BIN_DIR)/helper $(LFLAGS)

64:	64.o
	$(CC) 64.o -o $(BIN_DIR)/64 $(LFLAGS)

attr_manip:	attr_manip.o
	$(CC) attr_manip.o -o $(BIN_DIR)/attr_manip $(LFLAGS)

add_rem_attr:	add_rem_attr.o
	$(CC) add_rem_attr.o -o $(BIN_DIR)/add_rem_attr $(LFLAGS)
									
clean:
	rm -f *.o core
