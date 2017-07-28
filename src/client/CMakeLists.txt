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
        event_consumer.txx
        event_consumer.cxx
        zmqeventconsumer.cpp
        device/device_command_info.cpp)

add_library(client_objects OBJECT ${SOURCES})
target_include_directories(client_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(client_objects PRIVATE -fPIC)