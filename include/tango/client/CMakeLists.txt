set(HEADERS
        accessproxy.h
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
        AttributeProxy.h)

set(TEMPLATES event.tpp
        devapi_attr.tpp
        devapi_utils.tpp
        api_util.tpp
        devapi_pipe.tpp)

add_library(include_client_objects OBJECT ${HEADERS})
install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/client")