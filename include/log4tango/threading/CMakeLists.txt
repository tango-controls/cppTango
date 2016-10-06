set(HEADERS
        DummyThreads.hh
        PThreads.hh
        MSThreads.hh
        Threading.hh)

add_library(include_log4tango_threading_objects OBJECT ${HEADERS})

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/log4tango/threading)