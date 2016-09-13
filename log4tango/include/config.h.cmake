/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H

/* Define to 1 if you have the `ftime' function. */
#cmakedefine HAVE_FTIME

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine HAVE_GETTIMEOFDAY

/* define if the compiler has int64_t */
#cmakedefine HAVE_INT64_T

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Define to 1 if you have the <io.h> header file. */
#cmakedefine HAVE_IO_H

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H

/* define if the compiler implements namespaces */
#cmakedefine HAVE_NAMESPACES

/* define if the C library has snprintf */
#cmakedefine HAVE_SNPRINTF

/* define if the compiler has stringstream */
#cmakedefine HAVE_SSTREAM

/* define if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* define if threading is enabled */
#cmakedefine HAVE_THREADING

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H

/* Define to the sub-directory in which libtool stores uninstalled libraries.   */
#cmakedefine LT_OBJDIR

/* Name of package */
#cmakedefine PACKAGE "${CMAKE_PROJECT_NAME}"

/* Define to the address where bug reports for this package should be sent. */
#cmakedefine PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"

/* Define to the full name of this package. */
#cmakedefine PACKAGE_NAME "${CMAKE_PROJECT_NAME}"

/* Define to the full name and version of this package. */
#cmakedefine PACKAGE_STRING "@PACKAGE_STRING@"

/* Define to the one symbol short name of this package. */
#cmakedefine PACKAGE_TARNAME

/* Define to the home page for this package. */
#cmakedefine PACKAGE_URL "@PACKAGE_URL@"

/* Define to the version of this package. */
#cmakedefine PACKAGE_VERSION "${LIBRARY_VERSION}"

/* Define to 1 if you have the ANSI C header files. */
#cmakedefine STDC_HEADERS

/* define if pthread library is available */
#cmakedefine HAVE_PTHREAD_ATTR_GET_NP 1
#cmakedefine HAVE_PTHREAD_GETATTR_NP 1
#cmakedefine HAVE_PTHREAD_CREATE 1
#cmakedefine HAVE_PTHREAD_NP_H 1
#if defined(HAVE_PTHREAD_ATTR_GET_NP) || defined(HAVE_PTHREAD_GETATTR_NP) || defined(HAVE_PTHREAD_CREATE) || defined(HAVE_PTHREAD_NP_H)
#define USE_PTHREADS 1
#endif

/* Version number of package */
#cmakedefine VERSION "@LIBRARY_VERSION@"

/* If we're running on darwin/MacOsX */
#cmakedefine __darwin__

/* If we're running on FreeBSD */
#cmakedefine __freebsd__