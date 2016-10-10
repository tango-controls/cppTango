set(SOURCES backend_factory.cpp)
set(HEADERS backend_factory.hpp)

add_subdirectory(corba)

add_library(backend_objects OBJECT ${SOURCES} ${HEADERS})
target_include_directories(backend_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})