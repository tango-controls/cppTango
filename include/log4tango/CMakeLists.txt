set(HEADERS
        Appender.hh
        AppenderAttachable.hh
        LayoutAppender.hh
        FileAppender.hh
        RollingFileAppender.hh
        OstreamAppender.hh
        Layout.hh
        PatternLayout.hh
        XmlLayout.hh
        Logger.hh
        LogSeparator.hh
        LoggerStream.hh
        LogStream.hh
        LogStreambuf.hh
        LoggingEvent.hh
        Level.hh
        NDC.hh
        TimeStamp.hh
        Filter.hh
        Export.hh
        Portability.hh)

add_subdirectory(threading)

add_library(include_log4tango_objects OBJECT ${HEADERS} $<TARGET_OBJECTS:include_log4tango_threading_objects>)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/log4tango)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/log4tango)
#TODO if windows 32 install config-win32.h