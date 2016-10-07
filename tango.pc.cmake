prefix = @CMAKE_INSTALL_PREFIX@
exec_prefix = ${prefix}
includedir = ${prefix}/include
libdir = ${prefix}/@CMAKE_INSTALL_LIBDIR@

Name: @CMAKE_PROJECT_NAME@
Description: TANGO client/server API library
Version: @LIBRARY_VERSION@
Cflags: -std=c++11 -I${includedir}
Requires: omniDynamic4 >= 4.1.6, omniCOS4, libzmq
Libs: -L${libdir} -ltango