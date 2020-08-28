include(CheckIncludeFile)
include(CheckFunctionExists)
include(FindThreads)
include(CheckCXXSourceCompiles)
include(CheckTypeSize)

macro (LOG4TANGO_CHECK_INCLUDE_FILE header var)
    CHECK_INCLUDE_FILE("${header}"        LOG4TANGO_${var} )
endmacro()

LOG4TANGO_CHECK_INCLUDE_FILE("dlfcn.h"        HAVE_DLFCN_H)
LOG4TANGO_CHECK_INCLUDE_FILE("inttypes.h"     HAVE_INTTYPES_H)
LOG4TANGO_CHECK_INCLUDE_FILE("io.h"           HAVE_IO_H)
LOG4TANGO_CHECK_INCLUDE_FILE("memory.h"       HAVE_MEMORY_H)
LOG4TANGO_CHECK_INCLUDE_FILE("stdint.h"       HAVE_STDINT_H)
LOG4TANGO_CHECK_INCLUDE_FILE("stdlib.h"       HAVE_STDLIB_H)
LOG4TANGO_CHECK_INCLUDE_FILE("strings.h"      HAVE_STRINGS_H)
LOG4TANGO_CHECK_INCLUDE_FILE("string.h"       HAVE_STRING_H)
LOG4TANGO_CHECK_INCLUDE_FILE("sys/stat.h"     HAVE_SYS_STAT_H)
LOG4TANGO_CHECK_INCLUDE_FILE("sys/types.h"    HAVE_SYS_TYPES_H)
LOG4TANGO_CHECK_INCLUDE_FILE("unistd.h"       HAVE_UNISTD_H)

macro (LOG4TANGO_CHECK_FUNCTION_EXISTS function var)
    CHECK_FUNCTION_EXISTS("${function}"        LOG4TANGO_${var} )
endmacro()
#check function
LOG4TANGO_CHECK_FUNCTION_EXISTS(ftime         HAVE_FTIME)
LOG4TANGO_CHECK_FUNCTION_EXISTS(gettimeofday  HAVE_GETTIMEOFDAY)


#check threads
check_library_exists(pthread pthread_attr_get_np "" HAVE_PTHREAD_ATTR_GET_NP)
check_library_exists(pthread pthread_getattr_np "" HAVE_PTHREAD_GETATTR_NP)
check_library_exists(pthread pthread_create "" HAVE_PTHREAD_CREATE)
check_include_file("pthread.h;pthread_np.h" HAVE_PTHREAD_NP_H)

if(CMAKE_THREAD_LIBS_INIT)
    set(LOG4TANGO_HAVE_THREADING "/**/")
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
