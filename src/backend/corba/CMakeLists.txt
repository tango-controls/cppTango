set(SOURCES tango.cxx)
set(HEADERS corba.hxx)

add_library(corba_objects OBJECT ${SOURCES} ${HEADERS})
target_include_directories(corba_objects PRIVATE ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})