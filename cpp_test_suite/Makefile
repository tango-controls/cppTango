#######################################################################################################################
# DEFAULT VALUES (do not modify) ######################################################################################

# profile file name (the file containing custom settings - properties definitions)
PROFILE = default

# profile file extension (default *.properties)
PROFILE_EXT = properties

# directory where the profile files are stored
PROFILE_DIR = profiles

# cxx test suite executable name
RUNNER_NAME = runner

# files that will not be compiled (no need to provide the full path, just the filenames are sufficient)
EXCLUDE_FILES = 
EXCLUDE_FILES_DEFAULT = $(RUNNER_NAME).cpp compare_test.cpp cxx_suite_template.cpp #conf_devtest.cpp

# compile only these files (if not set, all files from the TEST_DIRS will be compiled)
COMPILE_FILES = 

# if set (to true) cxx test suite executable (RUNNER_NAME) will always be recompiled
# (useful when you want to add/remove cxx files to/from the RUNNER_NAME without modifying them - otherwise RUNNER_NAME would not recompile)
ALWAYS_RECOMPILE_CXX = 

# these directories will be scanned in search for *.cpp files (test suites to be compiled)
TEST_DIRS = . asyn event test_suite
TEST_DIRS_EXTRA = 

# compiler
CC = c++
    
# compiler flags
CXXFLAGS = -g -Wall -Wextra -DVALGRIND -D_PTHREADS -D_REENTRANT
CXXFLAGS_EXTRA = 

# linker flags
LFLAGS_EXTRA = 

# directory containing Tango header files
TANGO_INCLUDE_DIR = 

# directory containing Tango (shared) library
TANGO_LIB_DIR = 

# name of the Tango (shared) library
LIB_TANGO = -ltango

# directory where the binaries will be stored
BIN_DIR = bin

# directory where the object files will be stored
OBJ_DIR = obj

# END DEFAULT VALUES ##################################################################################################
#######################################################################################################################


#######################################################################################################################
# CONFIGURATION #######################################################################################################

# command line profile argument takes precedence over Makefile PROFILE variable
PROFILE := $(if $(profile),$(profile),$(PROFILE))

# load user defined profile, if not defined load the default profile
$(if $(wildcard $(PROFILE_DIR)/$(PROFILE).$(PROFILE_EXT)),,$(error "File " $(PROFILE_DIR)/$(PROFILE).$(PROFILE_EXT) " does not exist. Make sure to create such a file."))
include $(PROFILE_DIR)/$(PROFILE).$(PROFILE_EXT)

# append the defaults to the user settings
EXCLUDE_FILES += $(EXCLUDE_FILES_DEFAULT)
CXXFLAGS += $(CXXFLAGS_EXTRA)

# detect if linux
ifeq ($(shell uname),Linux)
    linux = true
endif

# check if compiler supports C++11; choose the compiler
ifdef linux
    # check that we have at least gcc 4.3 (for c++0x features)
    GCC_MAJOR_VERSION_GTEQ4 := $(shell expr `c++ -dumpversion | cut -f1 -d.` \>= 4)
    GCC_MINOR_VERSION_GTEQ3 := $(shell expr `c++ -dumpversion | cut -f2 -d.` \>= 3)
    ifeq ($(GCC_MAJOR_VERSION_GTEQ4),1)
        ifeq ($(GCC_MINOR_VERSION_GTEQ3),1)
            CXX11 = -std=c++0x
        endif
    endif
endif

# append C++11 support to compiler flags (if set)
CXXFLAGS += $(CXX11)

# include directories (if TANGO_INCLUDE_DIR not defined, TANGO_BASE/include is taken as default; if any cxx file is to be compiled, cxxtest/include is added to the directory list)
INCLUDE_DIRS = -I$(OMNI_BASE)/include \
			   $(if $(TANGO_INCLUDE_DIR), $(foreach path, $(TANGO_INCLUDE_DIR), -I$(path)),-I$(TANGO_BASE)/include) \
			   -I$(LOG4TANGO_BASE)/include/tango \
			   $(if $(CXX_DIR), -Icxxtest/include) \
			   -I$(ZMQ_BASE)/include \
			   -I. 

# library directories (if TANGO_LIB_DIR not defined, TANGO_BASE/lib is taken as default)
LIB_DIRS = -L $(OMNI_BASE)/lib \
           $(if $(TANGO_LIB_DIR), -L $(TANGO_LIB_DIR),-L $(TANGO_BASE)/lib) \
	   	   -L $(LOG4TANGO_BASE)/lib \
	   	   -L $(ZMQ_BASE)/lib

# linker flags
LFLAGS = -g $(LFLAGS_EXTRA) $(LIB_DIRS) $(LIB_TANGO) -llog4tango -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 -lzmq -lpthread -ldl

# finds all *.cpp files and filters out $(EXCLUDE_FILES); if $(COMPILE_FILES) defined lists only $(COMPILE_FILES) after filtering out $(EXCLUDE_FILES)
FILES_ALL = $(sort $(foreach file, $(foreach dir, $(TEST_DIRS), $(foreach filename, $(filter %.cpp, $(shell ls $(dir))), $(dir)/$(filename))), $(if $(filter-out $(notdir $(notdir $(EXCLUDE_FILES))), $(notdir $(file))), $(if $(COMPILE_FILES), $(if $(filter $(notdir $(COMPILE_FILES)), $(notdir $(file))), $(file)), $(file)))))
FILES_CPP = $(sort $(foreach file, $(FILES_ALL), $(if $(filter-out cxx_%, $(notdir $(file))), $(file))))
FILES_CXX = $(filter-out $(FILES_CPP), $(FILES_ALL))

# detect directory in which the cxx files are stored (if empty, no cxx files are to be compiled)
CXX_DIR = $(dir $(lastword $(FILES_CXX)))

# END CONFIGURATION ###################################################################################################
#######################################################################################################################

#######################################################################################################################
# RULES ###############################################################################################################

# target names (for non cxx files)
TARGETS = $(addprefix $(BIN_DIR)/, $(basename $(notdir $(FILES_CPP))))

# these rules do not create any files (functional rules)
.PHONY: all create_dirs clean recompile_cxx print

# invoke this target to build your test suites
all: $(if $(print_it), print) create_dirs $(if $(ALWAYS_RECOMPILE_CXX), recompile_cxx) $(TARGETS) $(if $(CXX_DIR), $(BIN_DIR)/$(RUNNER_NAME))
	@echo "=== all built successfully ==="

# compile compare_Test.cpp used in come cxx test suites
$(OBJ_DIR)/compare_test.o: $(CXX_DIR)compare_test.cpp
	@echo "---> compiling " $(@F)
	$(CC) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# cxx test suite executable (RUNNER_NAME) comprising all declared cxx files
$(BIN_DIR)/$(RUNNER_NAME): $(OBJ_DIR)/compare_test.o $(FILES_CXX)
	cxxtest/bin/cxxtestgen.py --template=cxxtest/template/tango_template.tpl -o $(CXX_DIR)$(RUNNER_NAME).cpp $(FILES_CXX)
	@echo "---> compiling " $(@F)
	$(CC) $(CXXFLAGS) $(INCLUDE_DIRS) -c $(CXX_DIR)$(RUNNER_NAME).cpp -o $(OBJ_DIR)/$(RUNNER_NAME).o
	@echo "===> linking " $(@F)
	$(CC) $(OBJ_DIR)/$(RUNNER_NAME).o $(OBJ_DIR)/compare_test.o -o $(BIN_DIR)/$(RUNNER_NAME) $(LFLAGS)

# dynamic target generator (macro)
define generate_targets
$(BIN_DIR)/$(basename $(notdir $(1))): $(OBJ_DIR)/$(basename $(notdir $(1))).o
	@echo "===> linking " $$<
	$(CC) $$< -o $(BIN_DIR)/$(basename $$(@F)) $(LFLAGS)
$(OBJ_DIR)/$(basename $(notdir $(1))).o: $(2)
	@echo "---> compiling " $$<
	$(CC) $(CXXFLAGS) $(INCLUDE_DIRS) -c $$< -o $$@
endef

# generate targets (for non cxx files)
$(foreach dependency, $(FILES_CPP), $(eval $(call generate_targets, $(basename $(notdir $(dependency))), $(dependency))))

# creates OBJ_DIR and BIN_DIR if they do not exist
create_dirs:
	$(shell mkdir -p $(OBJ_DIR))
	$(shell mkdir -p $(BIN_DIR))

# forces the RUNNER_NAME to recompile
recompile_cxx:
	$(foreach file, $(FILES_CXX), $(shell touch $(file)))

# print a variable
print_it = 
print:
	$(info [print] $(to_print))

# clean objects
clean:
	rm -f $(OBJ_DIR)/*.o core

# END RULES ###########################################################################################################
#######################################################################################################################