#
# +1 : ? : +1  == new interface that does not break old one
# +1 : ? : 0   == new interface that breaks old one
#  ? : ? : 0   == no new interfaces, but breaks apps
#  ? :+1 : ?   == just some internal changes, nothing breaks but might work
#                 better
# CURRENT : REVISION : AGE
set(LT_VERSION "5:1:0")

include(${CMAKE_CURRENT_SOURCE_DIR}/config/config.cmake)

add_definitions(-fPIC)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests)