set(SOURCES 
                Appender.cpp
                AppenderAttachable.cpp
                LayoutAppender.cpp
                FileAppender.cpp
                RollingFileAppender.cpp
                OstreamAppender.cpp
                Layout.cpp
                PatternLayout.cpp
                XmlLayout.cpp
                Logger.cpp
                LogSeparator.cpp
                LoggerStream.cpp
                LoggingEvent.cpp
                Level.cpp
                NDC.cpp
                Filter.cpp
                TimeStamp.cpp
                StringUtil.hh
                StringUtil.cpp
                DllMain.cpp
                DummyThreads.cpp
                MSThreads.cpp
                PThreads.cpp
                PortabilityImpl.hh
                PortabilityImpl.cpp)

add_library(log4tango_objects OBJECT ${SOURCES})