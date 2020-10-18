include(CheckIncludeFile)
include(CheckFunctionExists)
include(FindThreads)
include(CheckCXXSourceCompiles)
include(CheckTypeSize)

macro (LOG4TANGO_CHECK_INCLUDE_FILE header var)
    CHECK_INCLUDE_FILE("${header}"        LOG4TANGO_${var} )
endmacro()

LOG4TANGO_CHECK_INCLUDE_FILE("io.h"           HAVE_IO_H)
LOG4TANGO_CHECK_INCLUDE_FILE("unistd.h"       HAVE_UNISTD_H)

macro (LOG4TANGO_CHECK_FUNCTION_EXISTS function var)
    CHECK_FUNCTION_EXISTS("${function}"        LOG4TANGO_${var} )
endmacro()
#check function
LOG4TANGO_CHECK_FUNCTION_EXISTS(ftime         HAVE_FTIME)
LOG4TANGO_CHECK_FUNCTION_EXISTS(gettimeofday  HAVE_GETTIMEOFDAY)

if(NOT DEFINED Threads_FOUND)
  message(FATAL_ERROR "Could not find a suitable threading library")
endif()

#check types
check_type_size(int64_t INT64_SIZE)
if(WIN32)
    if(CMAKE_CL_64)
        set(LOG4TANGO_HAVE_INT64_T "/**/")
    endif()
else()
    if(${INT64_SIZE} EQUAL 8)
        set(LOG4TANGO_HAVE_INT64_T "/**/")
    endif()
endif()


configure_file(config/config.h.in ${PROJECT_BINARY_DIR}/log4tango/include/log4tango/config.h)
