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
                TimeStamp.hh
                Filter.hh
                Export.hh
                Portability.hh
                config.h
                config-win32.h)

install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/log4tango)

add_subdirectory(threading)