set(HEADER_FILES
                DummyThreads.hh
                StandardThreads.hh
                Threading.hh)

install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/log4tango/threading)
