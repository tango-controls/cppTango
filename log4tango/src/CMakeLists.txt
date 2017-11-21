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
IF(WIN32)
add_library(log4tango_objects_dyn OBJECT ${SOURCES})
add_library(log4tango_objects_sta OBJECT ${SOURCES})
target_compile_definitions(log4tango_objects_dyn PRIVATE "${dyn_defs}")
target_compile_definitions(log4tango_objects_sta PRIVATE "${static_defs}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_compile_options(log4tango_objects_sta PRIVATE "/MTd")
else()
target_compile_options(log4tango_objects_sta PRIVATE "/MT")
endif()
ELSE(WIN32)
add_library(log4tango_objects OBJECT ${SOURCES})
target_compile_options(log4tango_objects PRIVATE -fPIC)
ENDIF(WIN32)
