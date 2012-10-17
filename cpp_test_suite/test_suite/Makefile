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
BIN_SUITE_DIR = $(TANGO_HOME)/bin
else
TANGO_HOME = /home/taurel/tango/cppapi_develop
#TANGO_HOME = /home/taurel/tango/cppapi_900
#LIB_TANGO_DIR = $(TANGO_HOME)/lib
LIB_TANGO_DIR = $(TANGO_HOME)/../install/$(OS)/lib/debug

# binary target
BIN_SUITE_DIR = bin
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
			   -I../cxxtest/include \
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
			   -I../cxxtest/include \
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

CXXFLAGS = -g -w  $(CXX11) -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS) 
LIB_TANGO = -ltango
#LIB_TANGO = /home/taurel/tango/install/ubuntu1204/lib/debug/libtango.8.0.5.a -lgcov
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lzmq -lpthread -ldl
endif

OBJ_DIR = obj

.SUFFIXES:	.o .cpp
#.cpp.o:
#	$(CC) $(CXXFLAGS) -o $(OBJ_DIR)/$@.o -c $<
	
$(OBJ_DIR)/%.o: %.cpp
	@./cr_dir $(OBJ_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/$*.o
				
all: runner

conf:
	$(CC) $(CXXFLAGS) -c conf_devtest.cpp -o $(OBJ_DIR)/conf_devtest.o
	$(CC) $(OBJ_DIR)/conf_devtest.o -o $(BIN_SUITE_DIR)/conf_devtest $(LFLAGS)

execute_conf:
	$(BIN_SUITE_DIR)/conf_devtest $(conf_params)
	
test:
	$(BIN_SUITE_DIR)/runner dev/test/10 dev/test/11 dev/test/12 --loop=5 --fulldsname=devTest/api

compare_test: compare_test.cpp
	$(CC) $(CXXFLAGS) -c compare_test.cpp -o $(OBJ_DIR)/compare_test.o

runner: compare_test
	../cxxtest/bin/cxxtestgen.py --template=../cxxtest/template/tango_template.tpl -o runner.cpp \
	cxx_syntax.cpp \
	cxx_poll.cpp \
	cxx_poll_admin.cpp \
	cxx_dserver_misc.cpp \
	cxx_misc.cpp \
	cxx_always_hook.cpp \
	cxx_dserver_cmd.cpp \
	cxx_class_dev_signal.cpp \
	cxx_class_signal.cpp \
	cxx_signal.cpp \
	cxx_attr_write.cpp \
	cxx_attr_conf.cpp \
	cxx_attr_misc.cpp \
	cxx_attr.cpp \
	cxx_misc_util.cpp \
	cxx_templ_cmd.cpp \
	cxx_exception.cpp \
	cxx_cmd_query.cpp \
	cxx_seq_vec.cpp \
	cxx_test_state_on.cpp \
	cxx_cmd_types.cpp \
	cxx_blackbox.cpp \
	cxx_group.cpp \
	cxx_encoded.cpp \
	cxx_database.cpp \
	cxx_mem_attr.cpp
	$(CC) $(CXXFLAGS) -c runner.cpp -o $(OBJ_DIR)/runner.o
	$(CC) $(OBJ_DIR)/runner.o $(OBJ_DIR)/compare_test.o -o $(BIN_SUITE_DIR)/runner $(LFLAGS)

ifdef test_suites
runner_on_demand: compare_test
	../cxxtest/bin/cxxtestgen.py --template=../cxxtest/template/tango_template.tpl -o runner.cpp $(test_suites)
	$(CC) $(CXXFLAGS) -c runner.cpp -o $(OBJ_DIR)/runner.o
	$(CC) $(OBJ_DIR)/runner.o $(OBJ_DIR)/compare_test.o -o $(BIN_SUITE_DIR)/runner $(LFLAGS)
else
runner_on_demand: runner
endif
	
clean:
	rm -f $(OBJ_DIR)/*.o core
