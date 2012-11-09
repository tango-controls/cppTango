#######################################################################################################################
# 
# Parameters explained (with examples):
# 
# ! Assumption: all file names are unique !
# 
# Mandatory parameters:
# 
# TANGO_BASE = /usr/local/tango - directory where Tango include and lib directories are stored
# TANGO_INCLUDE_DIR = /usr/local/tango/include/client /usr/local/tango/include/server... - if defined, takes precedence
#       over the include directory located in TANGO_BASE; can contain more than one path
# TANGO_LIB_DIR = /usr/local/tango/lib - if defined, takes precedence over lib directory located in TANGO_BASE
# OMNI_BASE = /usr/local/omniORB - directory where omniORB include and lib directories are stored
# LOG4TANGO_BASE = /usr/local/log4tango - directory where log4tango include/tango and lib directories are stored
# ZMQ_BASE = /urs/local/zmq - directory where ZMQ include and lib directories are stored
# 
# Optional parameters:
# 
# TEST_DIRS = asyn event old_tests new_tests - directories where test suites (*.cpp and cxx_*.cpp) are located, all files in
#       these directories will be compiled (see also EXCLUDE_FILES and COMPILE_FILES); default values as in the example
# TEST_DIRS_EXTRA = my_tests1 my_tests2... - extra directories to be scanned in search for test suites; can be used to
#       add source directories to the default set of directories; default values - not set
# BIN_DIR = bin - directory where test suite binaries will be stored; default value as in the example
# OBJ_DIR = obj - directory where object files will be stored; default value as in the example
# 
# EXCLUDE_FILES = runner.cpp compare_test.cpp - do not compile these files; default values as in the example
# COMPILE_FILES = file1.cpp file2.cpp file3.cpp... - compile only listed files; default values - not set
# ALWAYS_RECOMPILE_CXX = true - if set to true forces recompilation of the cxx test suite runner; useful when combined
#       with COMPILE_FILES to select different cxx_ files to be incorporated in the runner, if ALWAYS_RECOMPILE_CXX
#       not set runner executable will not recompile if it already exists; default values - not set
# SORT_ALPHABETICALLY_CXX = true - has a meaning only when COMPILE_FILES is set; if set to true the cxx_ files will be
#       alphabetically sorted within the runner, otherwise the order defined by the user is kept; default values
#       - not set
# 
# CC = c++ - compiler; default value as in the example
# CXXFLAGS = -g -Wall -Wextra -DVALGRIND -D_PTHREADS -D_REENTRANT - compiler parameters; default values
#       as in the example
# CXXFLAGS_EXTRA = -O... - extra compiler options, will be appended to CXXFLAGS; default values - not set
# 
# LFLAGS_EXTRA = -static... - extra linker options; default values - not set
#
#######################################################################################################################

#EXCLUDE_FILES = #att_conf.cpp #64.cpp helper.cpp cxx_group.cpp
COMPILE_FILES = cxx_syntax.cpp cxx_attr.cpp #cxx_attprop.cpp
#SORT_ALPHABETICALLY_CXX = TRUE
ALWAYS_RECOMPILE_CXX = true
TEST_DIRS = new_tests

BIN_DIR = bin
OBJ_DIR = obj
TANGO_BASE = /home/trogucki/local/tango/8.1.0
TANGO_INCLUDE_DIR = /home/trogucki/workspace/cppapi_NextRelease/client /home/trogucki/workspace/cppapi_NextRelease/server
TANGO_LIB_DIR = /home/trogucki/local/tango/8.1.0/lib/debug
OMNI_BASE = /home/trogucki/local/omniORB
LOG4TANGO_BASE = /home/trogucki/local/log4tango
ZMQ_BASE = /home/trogucki/local/zmq