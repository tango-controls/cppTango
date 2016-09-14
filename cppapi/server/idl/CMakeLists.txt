set(SOURCES tangoSK.cpp
            tangoDynSK.cpp)

add_library(idl_objects OBJECT ${SOURCES} tango.h)

#TODO install