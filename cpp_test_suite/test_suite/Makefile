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
OS=ubuntu904
else
OS=ubuntu1004
endif
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
BIN_SUITE_DIR = $(TANGO_HOME)/bin
else
#TANGO_HOME = /usr/tango
TANGO_HOME = /home/taurel/tango/cppapi_develop
LIB_TANGO_DIR = $(TANGO_HOME)/lib
# binary target
BIN_SUITE_DIR = bin
endif

##############################################################################
#
# 		OmniORB base directory
#
##############################################################################

OMNI_BASE = /segfs/tango/ORB/omniORB4.1.5/$(BIN_DIR)

##############################################################################
#
# 		Log4tango base directory
#
##############################################################################

LOG4TANGO_BASE = /segfs/tango/tools/log4tango-release/log4tango4.0.4/$(BIN_DIR)

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
			   -I../cxxtest/include \
			   -I. 

LIB_DIRS = -L $(OMNI_BASE)/lib \
           -L $(LIB_TANGO_DIR)/debug \
	   	   -L $(LOG4TANGO_BASE)/lib
else
INCLUDE_DIRS = -I$(OMNI_BASE)/include \
		-I$(TANGO_HOME)/server \
		-I$(TANGO_HOME)/client \
		-I$(LOG4TANGO_BASE)/include/tango \
		-I../cxxtest/include \
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
CXXFLAGS = -g -DVALGRIND -D_PTHREADS -D_REENTRANT $(INCLUDE_DIRS) 
LIB_TANGO = -ltango
LFLAGS =  -g $(LIB_DIRS)  $(LIB_TANGO) -llog4tango \
	-lomniORB4 -lomniDynamic4 -lomnithread \
	-lCOS4 -lpthread -ldl
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
	$(CC) $(OBJ_DIR)/conf_devtest.o -o $(BIN_TANGO_DIR)/conf_devtest $(LFLAGS)
#	$(BIN_TANGO_DIR)/conf_devtest device1 device2 device3
	
test:
	$(BIN_SUITE_DIR)/runner dev/test/10 dev/test/11 dev/test/12 --loop=5 --fulldsname=devTest/api

compare_test: compare_test.cpp
	$(CC) $(CXXFLAGS) -c compare_test.cpp -o $(OBJ_DIR)/compare_test.o

runner: compare_test
	../cxxtest/bin/cxxtestgen.py --template=../cxxtest/template/tango_template.tpl -o runner.cpp \
	cxx_poll.cpp \
	cxx_poll_admin.cpp \
	cxx_dserver_misc.cpp \
	cxx_misc.cpp \
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
	cxx_blackbox.cpp
	$(CC) $(CXXFLAGS) -c runner.cpp -o $(OBJ_DIR)/runner.o
	$(CC) $(OBJ_DIR)/runner.o $(OBJ_DIR)/compare_test.o -o $(BIN_SUITE_DIR)/runner $(LFLAGS)
#	cxx_always_hook.cpp \
#	cxx_dserver_cmd.cpp
#	cxx_class_dev_signal.cpp \
#	cxx_class_signal.cpp \
#	cxx_signal.cpp 
runner_on_demand: compare_test
	../cxxtest/bin/cxxtestgen.py --template=../cxxtest/template/tango_template.tpl -o runner.cpp $(test_suites)
	$(CC) $(CXXFLAGS) -c runner.cpp -o $(OBJ_DIR)/runner.o
	$(CC) $(OBJ_DIR)/runner.o $(OBJ_DIR)/compare_test.o -o $(BIN_SUITE_DIR)/runner $(LFLAGS)
	
clean:
	rm -f $(OBJ_DIR)/*.o core
