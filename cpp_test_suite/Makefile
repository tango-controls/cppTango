##############################################################
#
#		Makefile to generate Tango test suite
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

CXXFLAGS = -g $(CXX11) -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS)
LIB_TANGO = -ltango
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lzmq -lpthread -ldl
endif

#.SUFFIXES:	.o .cpp
#.cpp.o:
#	$(CC) $(CXXFLAGS) -c $<
	
$(OBJS_DIR)/%.o: %.cpp
	@./cr_dir $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJS_DIR)/$*.o
	
				
all: new_devproxy multiple_new reconnect cmd_inout read_attr cmd_types \
     attr_types misc_devdata misc_devattr misc_devproxy write_attr \
     copy_devproxy obj_prop read_hist poll_met poll_except poll_attr print_data \
     print_data_hist size attr_proxy read_hist_ext write_attr_3 \
     ring_depth attr_misc state_attr rds mem_att attr_manip prop_list ds_cache sub_dev \
     locked_device lock restart_device unlock allowed_cmd w_r_attr  acc_right \
	 ConfEventBugClient att_conf Poll StopPoll poll_pool attr_conf_test


run: runtest
	./runtest $(platform)
	
on_demand: $(on_demand)

	
new_devproxy:	$(OBJS_DIR)/new_devproxy.o
	@./cr_dir $(BIN_TANGO_DIR)
	$(CC) $(OBJS_DIR)/new_devproxy.o -o $(BIN_TANGO_DIR)/new_devproxy $(LFLAGS)

multiple_new:	$(OBJS_DIR)/multiple_new.o
	$(CC) $(OBJS_DIR)/multiple_new.o -o $(BIN_TANGO_DIR)/multiple_new $(LFLAGS)

reconnect:	$(OBJS_DIR)/reconnect.o
	$(CC) $(OBJS_DIR)/reconnect.o -o $(BIN_TANGO_DIR)/reconnect $(LFLAGS)

reconnect_attr:	$(OBJS_DIR)/reconnect_attr.o
	$(CC) $(OBJS_DIR)/reconnect_attr.o -o $(BIN_TANGO_DIR)/reconnect_attr $(LFLAGS)
	
cmd_inout:	$(OBJS_DIR)/cmd_inout.o
	$(CC) $(OBJS_DIR)/cmd_inout.o -o $(BIN_TANGO_DIR)/cmd_inout $(LFLAGS)

read_attr:	$(OBJS_DIR)/read_attr.o
	$(CC) $(OBJS_DIR)/read_attr.o -o $(BIN_TANGO_DIR)/read_attr $(LFLAGS)

cmd_types:	$(OBJS_DIR)/cmd_types.o
	$(CC) $(OBJS_DIR)/cmd_types.o -o $(BIN_TANGO_DIR)/cmd_types $(LFLAGS)

attr_types:	$(OBJS_DIR)/attr_types.o
	$(CC) $(OBJS_DIR)/attr_types.o -o $(BIN_TANGO_DIR)/attr_types $(LFLAGS)

misc_devdata:	$(OBJS_DIR)/misc_devdata.o
	$(CC) $(OBJS_DIR)/misc_devdata.o -o $(BIN_TANGO_DIR)/misc_devdata $(LFLAGS)
		
misc_devattr:	$(OBJS_DIR)/misc_devattr.o
	$(CC) $(OBJS_DIR)/misc_devattr.o -o $(BIN_TANGO_DIR)/misc_devattr $(LFLAGS)
						
misc_devproxy:	$(OBJS_DIR)/misc_devproxy.o
	$(CC) $(OBJS_DIR)/misc_devproxy.o -o $(BIN_TANGO_DIR)/misc_devproxy $(LFLAGS)

write_attr:	$(OBJS_DIR)/write_attr.o
	$(CC) $(OBJS_DIR)/write_attr.o -o $(BIN_TANGO_DIR)/write_attr $(LFLAGS)
		
copy_devproxy:	$(OBJS_DIR)/copy_devproxy.o
	$(CC) $(OBJS_DIR)/copy_devproxy.o -o $(BIN_TANGO_DIR)/copy_devproxy $(LFLAGS)

obj_prop:	$(OBJS_DIR)/obj_prop.o
	$(CC) $(OBJS_DIR)/obj_prop.o -o $(BIN_TANGO_DIR)/obj_prop $(LFLAGS)

read_hist:	$(OBJS_DIR)/read_hist.o
	$(CC) $(OBJS_DIR)/read_hist.o -o $(BIN_TANGO_DIR)/read_hist $(LFLAGS)
	
att_conf:	$(OBJS_DIR)/att_conf.o
	$(CC) $(OBJS_DIR)/att_conf.o -o $(BIN_TANGO_DIR)/att_conf $(LFLAGS)

poll_met:	$(OBJS_DIR)/poll_met.o
	$(CC) $(OBJS_DIR)/poll_met.o -o $(BIN_TANGO_DIR)/poll_met $(LFLAGS)

poll_except:	$(OBJS_DIR)/poll_except.o
	$(CC) $(OBJS_DIR)/poll_except.o -o $(BIN_TANGO_DIR)/poll_except $(LFLAGS)
	
poll_attr:	$(OBJS_DIR)/poll_attr.o
	$(CC) $(OBJS_DIR)/poll_attr.o -o $(BIN_TANGO_DIR)/poll_attr $(LFLAGS)

print_data:	$(OBJS_DIR)/print_data.o
	$(CC) $(OBJS_DIR)/print_data.o -o $(BIN_TANGO_DIR)/print_data $(LFLAGS)

print_data_hist:	$(OBJS_DIR)/print_data_hist.o
	$(CC) $(OBJS_DIR)/print_data_hist.o -o $(BIN_TANGO_DIR)/print_data_hist $(LFLAGS)
						
size:	$(OBJS_DIR)/size.o
	$(CC) $(OBJS_DIR)/size.o -o $(BIN_TANGO_DIR)/size $(LFLAGS)

write_attr_3:	$(OBJS_DIR)/write_attr_3.o
	$(CC) $(OBJS_DIR)/write_attr_3.o -o $(BIN_TANGO_DIR)/write_attr_3 $(LFLAGS)

read_hist_ext:	$(OBJS_DIR)/read_hist_ext.o
	$(CC) $(OBJS_DIR)/read_hist_ext.o -o $(BIN_TANGO_DIR)/read_hist_ext $(LFLAGS)

attr_proxy:	$(OBJS_DIR)/attr_proxy.o
	$(CC) $(OBJS_DIR)/attr_proxy.o -o $(BIN_TANGO_DIR)/attr_proxy $(LFLAGS)
				
ring_depth:	$(OBJS_DIR)/ring_depth.o
	$(CC) $(OBJS_DIR)/ring_depth.o -o $(BIN_TANGO_DIR)/ring_depth $(LFLAGS)

attr_misc:	$(OBJS_DIR)/attr_misc.o
	$(CC) $(OBJS_DIR)/attr_misc.o -o $(BIN_TANGO_DIR)/attr_misc $(LFLAGS)

state_attr:	$(OBJS_DIR)/state_attr.o
	$(CC) $(OBJS_DIR)/state_attr.o -o $(BIN_TANGO_DIR)/state_attr $(LFLAGS)

attr_conf_test:	$(OBJS_DIR)/attr_conf_test.o
	$(CC) $(OBJS_DIR)/attr_conf_test.o -o $(BIN_TANGO_DIR)/attr_conf_test $(LFLAGS)

rds:	$(OBJS_DIR)/rds.o
	$(CC) $(OBJS_DIR)/rds.o -o $(BIN_TANGO_DIR)/rds $(LFLAGS)

mem_att:	$(OBJS_DIR)/mem_att.o
	$(CC) $(OBJS_DIR)/mem_att.o -o $(BIN_TANGO_DIR)/mem_att $(LFLAGS)

helper:	$(OBJS_DIR)/helper.o
	$(CC) $(OBJS_DIR)/helper.o -o $(BIN_TANGO_DIR)/helper $(LFLAGS)

64:	$(OBJS_DIR)/64.o
	$(CC) $(OBJS_DIR)/64.o -o $(BIN_TANGO_DIR)/64 $(LFLAGS)

attr_manip:	$(OBJS_DIR)/attr_manip.o
	$(CC) $(OBJS_DIR)/attr_manip.o -o $(BIN_TANGO_DIR)/attr_manip $(LFLAGS)

add_rem_attr:	$(OBJS_DIR)/add_rem_attr.o
	$(CC) $(OBJS_DIR)/add_rem_attr.o -o $(BIN_TANGO_DIR)/add_rem_attr $(LFLAGS)
	
add_rem_dev:	$(OBJS_DIR)/add_rem_dev.o
	$(CC) $(OBJS_DIR)/add_rem_dev.o -o $(BIN_TANGO_DIR)/add_rem_dev $(LFLAGS)
									
prop_list:	$(OBJS_DIR)/prop_list.o
	$(CC) $(OBJS_DIR)/prop_list.o -o $(BIN_TANGO_DIR)/prop_list $(LFLAGS)

ds_cache:	$(OBJS_DIR)/ds_cache.o
	$(CC) $(OBJS_DIR)/ds_cache.o -o $(BIN_TANGO_DIR)/ds_cache $(LFLAGS)

lock:	$(OBJS_DIR)/lock.o
	$(CC) $(OBJS_DIR)/lock.o -o $(BIN_TANGO_DIR)/lock $(LFLAGS)

locked_device:	$(OBJS_DIR)/locked_device.o
	$(CC) $(OBJS_DIR)/locked_device.o -o $(BIN_TANGO_DIR)/locked_device $(LFLAGS)

restart_device:	$(OBJS_DIR)/restart_device.o
	$(CC) $(OBJS_DIR)/restart_device.o -o $(BIN_TANGO_DIR)/restart_device $(LFLAGS)

unlock:	$(OBJS_DIR)/unlock.o
	$(CC) $(OBJS_DIR)/unlock.o -o $(BIN_TANGO_DIR)/unlock $(LFLAGS)

allowed_cmd:	$(OBJS_DIR)/allowed_cmd.o
	$(CC) $(OBJS_DIR)/allowed_cmd.o -o $(BIN_TANGO_DIR)/allowed_cmd $(LFLAGS)

w_r_attr:	$(OBJS_DIR)/w_r_attr.o
	$(CC) $(OBJS_DIR)/w_r_attr.o -o $(BIN_TANGO_DIR)/w_r_attr $(LFLAGS)

acc_right:	$(OBJS_DIR)/acc_right.o
	$(CC) $(OBJS_DIR)/acc_right.o -o $(BIN_TANGO_DIR)/acc_right $(LFLAGS)

sub_dev:	$(OBJS_DIR)/sub_dev.o
	$(CC) $(OBJS_DIR)/sub_dev.o -o $(BIN_TANGO_DIR)/sub_dev $(LFLAGS)

ConfEventBugClient:	$(OBJS_DIR)/ConfEventBugClient.o
	$(CC) $(OBJS_DIR)/ConfEventBugClient.o -o $(BIN_TANGO_DIR)/ConfEventBugClient $(LFLAGS)

Poll:	$(OBJS_DIR)/Poll.o
	$(CC) $(OBJS_DIR)/Poll.o -o $(BIN_TANGO_DIR)/Poll $(LFLAGS)

StopPoll:	$(OBJS_DIR)/StopPoll.o
	$(CC) $(OBJS_DIR)/StopPoll.o -o $(BIN_TANGO_DIR)/StopPoll $(LFLAGS)

poll_pool:	$(OBJS_DIR)/poll_pool.o
	$(CC) $(OBJS_DIR)/poll_pool.o -o $(BIN_TANGO_DIR)/poll_pool $(LFLAGS)
	
clean:
	rm -f $(OBJS_DIR)/*.o core
