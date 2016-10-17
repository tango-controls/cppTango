set(SOURCES DevVarDoubleArray.cxx)

add_library(backend_objects OBJECT ${SOURCES})
target_include_directories(backend_objects PRIVATE ${PROJECT_SOURCE_DIR}/generated)
target_compile_options(backend_objects PRIVATE -fPIC)