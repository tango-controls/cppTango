include(CheckIncludeFile)
include(CheckFunctionExists)
include(FindThreads)

#TODO config.h.in stamp-h.in
CHECK_INCLUDE_FILE("dlfcn.h"        HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("inttypes.h"     HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("io.h"           HAVE_IO_H)
CHECK_INCLUDE_FILE("memory.h"       HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("stdint.h"       HAVE_STDINT_H)
CHECK_INCLUDE_FILE("stdlib.h"       HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("strings.h"      HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("string.h"       HAVE_STRING_H)
CHECK_INCLUDE_FILE("sys/stat.h"     HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/types.h"    HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("unistd.h"       HAVE_UNISTD_H)

#check function
CHECK_FUNCTION_EXISTS(ftime         HAVE_FTIME)
CHECK_FUNCTION_EXISTS(gettimeofday  HAVE_GETTIMEOFDAY)

#check compiler feature

#check threads
check_library_exists(pthread pthread_attr_get_np "" HAVE_PTHREAD_ATTR_GET_NP)
check_library_exists(pthread pthread_getattr_np "" HAVE_PTHREAD_GETATTR_NP)
check_library_exists(pthread pthread_create "" HAVE_PTHREAD_CREATE)
check_include_file("pthread.h;pthread_np.h" HAVE_PTHREAD_NP_H)

configure_file(config.h.cmake ${PROJECT_BINARY_DIR}/log4tango/include/log4tango/config.h)

add_subdirectory(log4tango)