# Configure paths for LOG4TANGO
# Owen Taylor     97-11-3

dnl AM_PATH_LOG4TANGO([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for LOG4TANGO, and define LOG4TANGO_CFLAGS and LOG4TANGO_LIBS
dnl
AC_DEFUN(AM_PATH_LOG4TANGO,
[dnl 
dnl Get the cflags and libraries from the log4tango-config script
dnl
AC_ARG_WITH(log4tango-prefix,[  --with-log4tango-prefix=PFX   Prefix where LOG4TANGO is installed (optional)],
            log4tango_config_prefix="$withval", log4tango_config_prefix="")
AC_ARG_WITH(log4tango-exec-prefix,[  --with-log4tango-exec-prefix=PFX Exec prefix where LOG4TANGO is installed (optional)],
            log4tango_config_exec_prefix="$withval", log4tango_config_exec_prefix="")
AC_ARG_ENABLE(log4tangotest, [  --disable-log4tangotest       Do not try to compile and run a test LOG4TANGO program],
		    , enable_log4tangotest=yes)

  if test x$log4tango_config_exec_prefix != x ; then
     log4tango_config_args="$log4tango_config_args --exec-prefix=$log4tango_config_exec_prefix"
     if test x${LOG4TANGO_CONFIG+set} != xset ; then
        LOG4TANGO_CONFIG=$log4tango_config_exec_prefix/bin/log4tango-config
     fi
  fi
  if test x$log4tango_config_prefix != x ; then
     log4tango_config_args="$log4tango_config_args --prefix=$log4tango_config_prefix"
     if test x${LOG4TANGO_CONFIG+set} != xset ; then
        LOG4TANGO_CONFIG=$log4tango_config_prefix/bin/log4tango-config
     fi
  fi

  AC_PATH_PROG(LOG4TANGO_CONFIG, log4tango-config, no)
  min_log4tango_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for LOG4TANGO - version >= $min_log4tango_version)
  no_log4tango=""
  if test "$LOG4TANGO_CONFIG" = "no" ; then
    no_log4tango=yes
  else
    LOG4TANGO_CFLAGS=`$LOG4TANGO_CONFIG $log4tango_config_args --cflags`
    LOG4TANGO_LIBS=`$LOG4TANGO_CONFIG $log4tango_config_args --libs`
    log4tango_config_major_version=`$LOG4TANGO_CONFIG $log4tango_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    log4tango_config_minor_version=`$LOG4TANGO_CONFIG $log4tango_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    log4tango_config_micro_version=`$LOG4TANGO_CONFIG $log4tango_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_log4tangotest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $LOG4TANGO_CFLAGS"
      CXXFLAGS="$CXXFLAGS $LOG4TANGO_CFLAGS"
      LIBS="$LOG4TANGO_LIBS $LIBS"
dnl
dnl Now check if the installed LOG4TANGO is sufficiently new. (Also sanity
dnl checks the results of log4tango-config to some extent
dnl
      rm -f conf.log4tangotest
      AC_TRY_RUN([
#include <log4tango/Category.hh>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int log4tango_major, log4tango_minor, log4tango_micro;
  int major, minor, micro;
  char *tmp_version;
  char *tmp_log4tango_version;

  system ("touch conf.log4tangotest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = strdup("$min_log4tango_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_log4tango_version");
     exit(1);
   }

   /* HP/UX 9 (%@#!) writes to sscanf strings */
   tmp_log4tango_version = strdup(LOG4TANGO_VERSION);
   if (sscanf(tmp_log4tango_version, "%d.%d.%d", &log4tango_major, &log4tango_minor, &log4tango_micro) != 3) {
     printf("%s, bad log4tango version string\n", LOG4TANGO_VERSION);
     exit(1);
   }

   if ((log4tango_major > major) ||
      ((log4tango_major == major) && (log4tango_minor > minor)) ||
      ((log4tango_major == major) && (log4tango_minor == minor) && (log4tango_micro >= micro)))
   {
        return 0;
   }
   else
   {
        printf("\n*** An old version of LOG4TANGO (%d.%d.%d) was found.\n",
               log4tango_major, log4tango_minor, log4tango_micro);
        printf("*** You need a version of LOG4TANGO newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the log4tango-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of LOG4TANGO, but you can also set the LOG4TANGO_CONFIG environment to point to the\n");
        printf("*** correct copy of log4tango-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
   }

   log4tango::Category& log = log4tango::Category::getRoot();
   return 1;
}
],, no_log4tango=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_log4tango" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
if test "$LOG4TANGO_CONFIG" = "no" ; then
echo "*** The log4tango-config script installed by LOG4TANGO could not be found"
       echo "*** If LOG4TANGO was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the LOG4TANGO_CONFIG environment variable to the"
       echo "*** full path to log4tango-config."
     else
       if test -f conf.log4tangotest ; then
        :
       else
          echo "*** Could not run LOG4TANGO test program, checking why..."
          CFLAGS="$CFLAGS $LOG4TANGO_CFLAGS"
          CXXFLAGS="$CXXFLAGS $LOG4TANGO_CFLAGS"
          LIBS="$LIBS $LOG4TANGO_LIBS"
          AC_TRY_LINK([
#include <log4tango/Category.hh>
],      [ log4tango::Category cat("a_cat"); return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding LOG4TANGO or finding the wrong"
          echo "*** version of LOG4TANGO. If it is not finding LOG4TANGO, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means LOG4TANGO was incorrectly installed"
          echo "*** or that you have moved LOG4TANGO since it was installed. In the latter case, you"
          echo "*** may want to edit the log4tango-config script: $LOG4TANGO_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     LOG4TANGO_CFLAGS=""
     LOG4TANGO_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(LOG4TANGO_CFLAGS)
  AC_SUBST(LOG4TANGO_LIBS)
  rm -f conf.log4tangotest
])
