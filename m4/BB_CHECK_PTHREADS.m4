AC_DEFUN([BB_CHECK_PTHREADS],
[AC_SEARCH_LIBS(pthread_key_create,pthread,
  [AC_DEFINE(HAVE_THREADING,,[define if threading is enabled])
   AC_DEFINE(USE_PTHREADS,,[define if pthread library is available])
    case $host_os in
    linux*)
      CFLAGS="$CFLAGS -D_REENTRANT"
      CXXFLAGS="$CXXFLAGS -D_REENTRANT"
      ;;
    solaris*)
      if test x$GCC = x ; 
      then 
      	 CFLAGS="$CFLAGS -mt -D_POSIX_PTHREAD_SEMANTICS"
         CXXFLAGS="$CXXFLAGS -mt -D_POSIX_PTHREAD_SEMANTICS"
      else
         CFLAGS="$CFLAGS -D_REENTRANT"
         CXXFLAGS="$CXXFLAGS -D_REENTRANT"
      fi
      ;;
    hpux*)
      CFLAGS="$CFLAGS -AA -mt +inst_close"
      CXXFLAGS="$CXXFLAGS -AA -mt +inst_close"
      ;;
    esac],
AC_MSG_ERROR([pthreads not found]))
])
