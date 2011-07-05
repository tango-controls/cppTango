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
BIN_TANGO_DIR = $(TANGO_HOME)/bin
else
TANGO_HOME = /usr/tango
LIB_TANGO_DIR = $(TANGO_HOME)/lib
# binary target
BIN_TANGO_DIR = bin
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
			   -I. 

LIB_DIRS = -L $(OMNI_BASE)/lib \
           -L $(LIB_TANGO_DIR)/debug \
	   	   -L $(LOG4TANGO_BASE)/lib
else
INCLUDE_DIRS = -I$(OMNI_BASE)/include \
			   -I$(TANGO_HOME)/include \
			   -I$(LOG4TANGO_BASE)/include/tango \
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
	-lCOS4 -lpthread -ldl -lcppunit
endif

OBJ_DIR = obj
BIN_SUITE_DIR = bin

.SUFFIXES:	.o .cpp
#.cpp.o:
#	$(CC) $(CXXFLAGS) -o $(OBJ_DIR)/$@.o -c $<
	
$(OBJ_DIR)/%.o: %.cpp
	@./cr_dir $(OBJ_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/$*.o
				
all: runner

test:
	$(BIN_SUITE_DIR)/runner dev/test/10 dev/test/11 dev/test/12 --loop=5 --fulldsname=devTest/api

runner: 
	../cxxtest/bin/cxxtestgen.py --template=../cxxtest/template/tango_template.tpl -o runner.cpp \
	cxx_attr.cpp \
	$(CC) $(CXXFLAGS) -c runner.cpp -o $(OBJ_DIR)/runner.o
	$(CC) $(OBJ_DIR)/runner.o -o $(BIN_SUITE_DIR)/runner $(LFLAGS)

clean:
	rm -f $(OBJ_DIR)/*.o core
