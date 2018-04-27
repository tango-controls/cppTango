prefix = @CMAKE_INSTALL_PREFIX@
exec_prefix = ${prefix}
includedir = ${prefix}/include/tango
libdir = ${prefix}/@CMAKE_INSTALL_LIBDIR@

Name: @CMAKE_PROJECT_NAME@
Description: Tango client/server API library
Version: @LIBRARY_VERSION@
Cflags: -I${includedir}
Requires: libzmq omniORB4 omniCOS4 omniDynamic4
Libs: -L${libdir} -ltango -lzmq -lomniORB4 -lomnithread -lCOS4 -lomniDynamic4