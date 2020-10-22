set(HEADER_FILES
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
                Filter.hh
                Export.hh
                Portability.hh)

add_subdirectory(threading)

install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/log4tango)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/log4tango)
#TODO if windows 32 install config-win32.h
