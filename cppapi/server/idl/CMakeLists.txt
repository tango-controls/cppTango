set(SOURCES tangoSK.cpp
            tangoDynSK.cpp)

add_library(idl_objects OBJECT ${SOURCES})
target_include_directories(idl_objects PUBLIC ${OMNIORB_PKG_INCLUDE_DIRS})
target_compile_options(idl_objects PUBLIC ${OMNIORB_PKG_CFLAGS_OTHER})

#TODO install