set(SOURCES dbapi_class.cpp
            dbapi_server.cpp
            dbapi_datum.cpp
            dbapi_base.cpp
            dbapi_device.cpp
            dbapi_history.cpp
            dbapi_attribute.cpp
            dbapi_cache.cpp
            dbapi_serverdata.cpp
            devapi_attr.cpp
            devapi_base.cpp
            devapi_data.cpp
            devapi_datahist.cpp
            devapi_utils.cpp
            devapi_pipe.cpp
            api_util.cpp
            asynreq.cpp
            cbthread.cpp
            proxy_asyn.cpp
            proxy_asyn_cb.cpp
            attr_proxy.cpp
            group.cpp
            filedatabase.cpp
            apiexcept.cpp
            accessproxy.cpp
            lockthread.cpp
            event.cpp
            eventkeepalive.cpp
            eventqueue.cpp
            notifdeventconsumer.cpp
            zmqeventconsumer.cpp)

set(HEADERS accessproxy.h
            apiexcept.h
            cbthread.h
            dbapi.h
            devapi.h
            devasyn.h
            event.h
            eventconsumer.h
            filedatabase.h
            group.h
            lockthread.h
            Database.h
            DbDevice.h
            ApiUtil.h
            DeviceData.h
            DeviceAttribute.h
            DevicePipe.h
            Connection.h
            DeviceProxy.h
            AttributeProxy.h
            event.tpp
            devapi_attr.tpp
            devapi_utils.tpp
            api_util.tpp
            devapi_pipe.tpp)

add_subdirectory(helpers)
add_library(client_objects OBJECT ${SOURCES})
add_dependencies(client_objects idl_objects)

if(WIN32)
    target_compile_definitions(client_objects PRIVATE "${windows_defs}")

    if(USE_PCH AND NOT BUILD_SHARED_LIBS)
        # shared cannot be compiled with PCH due to __@_PchSym_ collision
        tango_target_pch(client_objects)
    endif()
else()
    target_compile_options(client_objects PRIVATE -fPIC)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(client_objects PRIVATE _REENTRANT _TANGO_LIB)
    else()
        # Do not define _TANGO_LIB when compiling Tango debug library on Linux
        # in order to keep the same behaviour as in the past:
        # cout messages are not redirected to the logging system but are
        # instead displayed directly on the console
        target_compile_definitions(client_objects PRIVATE _REENTRANT)
    endif()
    if(USE_PCH)
        tango_target_pch(client_objects tango_pch)
    endif()
endif()
install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango")
