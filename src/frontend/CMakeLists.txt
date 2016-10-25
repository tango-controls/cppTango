set(SOURCES
        tango_orb.cxx
        tango_orb_provider.cxx)

add_library(frontend_objects OBJECT ${SOURCES})
target_compile_options(frontend_objects PRIVATE -fPIC)