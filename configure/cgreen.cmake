# - Try to find cgreen
# Once done this will define
#  LIBXML2_FOUND - System has cgreen
#  LIBXML2_INCLUDE_DIRS - The cgreen include directories
#  LIBXML2_LIBRARIES - The libraries needed to use cgreen
#  LIBXML2_DEFINITIONS - Compiler switches required for using cgreen

find_path(CGREEN_INCLUDE_DIR cgreen/cgreen.h)

find_library(CGREEN_LIBRARY cgreen)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(cgreen  DEFAULT_MSG
        CGREEN_LIBRARY CGREEN_INCLUDE_DIR)

mark_as_advanced(CGREEN_INCLUDE_DIR CGREEN_LIBRARY )

set(CGREEN_LIBRARIES ${CGREEN_LIBRARY} )
set(CGREEN_INCLUDE_DIRS ${CGREEN_INCLUDE_DIR} )
