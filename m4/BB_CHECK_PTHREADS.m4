AC_DEFUN([BB_CHECK_PTHREADS],
[AC_SEARCH_LIBS(pthread_key_create,pthread,
  [AC_DEFINE(HAVE_THREADING,,[define if threading is enabled])
   AC_DEFINE(USE_PTHREADS,,[define if pthread library is available])
    case $host_os in
    solaris*)
      CFLAGS="$CFLAGS -mt -D_POSIX_PTHREAD_SEMANTICS"
      CXXFLAGS="$CXXFLAGS -mt -D_POSIX_PTHREAD_SEMANTICS"
      ;;
    linux*)
      CFLAGS="$CFLAGS -pthread -D_PTHREADS"
      CXXFLAGS="$CXXFLAGS -pthread -D_PTHREADS"
      ;;
    esac],
AC_MSG_ERROR([pthreads not found]))
])
