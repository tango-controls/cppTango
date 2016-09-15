set(SOURCES tangoSK.cpp
            tangoDynSK.cpp)

add_library(idl_objects OBJECT ${SOURCES} tango.h)

#TODO generate source code from idl

install(FILES tango.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/idl")