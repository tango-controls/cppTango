set(SOURCES DevVarDoubleArray.cxx)

add_library(frontend_objects OBJECT ${SOURCES})
target_include_directories(frontend_objects PRIVATE ${PROJECT_SOURCE_DIR}/generated)
target_compile_options(frontend_objects PRIVATE -fPIC)