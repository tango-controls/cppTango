#
# +1 : ? : +1  == new interface that does not break old one
# +1 : ? : 0   == new interface that breaks old one
#  ? : ? : 0   == no new interfaces, but breaks apps
#  ? :+1 : ?   == just some internal changes, nothing breaks but might work
#                 better
# CURRENT : REVISION : AGE
set (LOG4TANGO_PACKAGE_URL "https://github.com/tango-controls/cppTango")
set (LOG4TANGO_PACKAGE_BUGREPORT "https://github.com/tango-controls/cppTango/issues")

include(${CMAKE_CURRENT_SOURCE_DIR}/config/config.cmake)

add_subdirectory(include)
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
