set(SOURCES
        orb.cxx)


add_library(backend_objects OBJECT ${SOURCES})
target_compile_options(backend_objects PRIVATE -fPIC)