set(HEADER_FILES
                DummyThreads.hh
                PThreads.hh
                MSThreads.hh
                Threading.hh)

install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/log4tango/threading)