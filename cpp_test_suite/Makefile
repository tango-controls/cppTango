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
TANGO_HOME = /segfs/tango/cppapi_7_2_1

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
CC = /opt/SUNWspro/bin/CC
BIN_DIR = solaris9
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
#	       -I/segfs/tango/release/ubuntu904/include \
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
CXXFLAGS = -g -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
#CXXFLAGS = -g -DCOMPAT -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lpthread -ldl
endif

.SUFFIXES:	.o .cpp
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<
	
	
				
all: new_devproxy multiple_new reconnect cmd_inout read_attr cmd_types \
     attr_types misc_devdata misc_devattr misc_devproxy write_attr \
     copy_devproxy obj_prop read_hist poll_met poll_except poll_attr print_data \
     print_data_hist syntax size attr_proxy read_hist_ext write_attr_3 \
     ring_depth attr_misc state_attr rds mem_att attr_manip prop_list ds_cache sub_dev \
     locked_device lock restart_device unlock allowed_cmd w_r_attr  acc_right sub_dev \
	 ConfEventBugClient 

	
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
	
att_conf:	att_conf.o
	$(CC) att_conf.o -o $(BIN_DIR)/att_conf $(LFLAGS)

poll_met:	poll_met.o
	$(CC) poll_met.o -o $(BIN_DIR)/poll_met $(LFLAGS)

poll_except:	poll_except.o
	$(CC) poll_except.o -o $(BIN_DIR)/poll_except $(LFLAGS)
	
poll_attr:	poll_attr.o
	$(CC) poll_attr.o -o $(BIN_DIR)/poll_attr $(LFLAGS)

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
	
add_rem_dev:	add_rem_dev.o
	$(CC) add_rem_dev.o -o $(BIN_DIR)/add_rem_dev $(LFLAGS)
									
prop_list:	prop_list.o
	$(CC) prop_list.o -o $(BIN_DIR)/prop_list $(LFLAGS)

ds_cache:	ds_cache.o
	$(CC) ds_cache.o -o $(BIN_DIR)/ds_cache $(LFLAGS)

lock:	lock.o
	$(CC) lock.o -o $(BIN_DIR)/lock $(LFLAGS)

locked_device:	locked_device.o
	$(CC) locked_device.o -o $(BIN_DIR)/locked_device $(LFLAGS)

restart_device:	restart_device.o
	$(CC) restart_device.o -o $(BIN_DIR)/restart_device $(LFLAGS)

unlock:	unlock.o
	$(CC) unlock.o -o $(BIN_DIR)/unlock $(LFLAGS)

allowed_cmd:	allowed_cmd.o
	$(CC) allowed_cmd.o -o $(BIN_DIR)/allowed_cmd $(LFLAGS)

w_r_attr:	w_r_attr.o
	$(CC) w_r_attr.o -o $(BIN_DIR)/w_r_attr $(LFLAGS)

acc_right:	acc_right.o
	$(CC) acc_right.o -o $(BIN_DIR)/acc_right $(LFLAGS)

sub_dev:	sub_dev.o
	$(CC) sub_dev.o -o $(BIN_DIR)/sub_dev $(LFLAGS)

ConfEventBugClient:	ConfEventBugClient.o
	$(CC) ConfEventBugClient.o -o $(BIN_DIR)/ConfEventBugClient $(LFLAGS)
	
clean:
	rm -f *.o core
