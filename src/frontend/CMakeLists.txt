set(SOURCES
        orb.cxx
        )

add_library(frontend_objects OBJECT ${SOURCES})
target_compile_options(frontend_objects PRIVATE -fPIC)