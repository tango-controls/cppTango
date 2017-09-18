set(SOURCES)
set(HEADERS
        zmq_event_subscription_change_response.h
        zmq_event_suscription_change_request.h
        zmq_event_subscription_info.h
        zmq_event_subscription_info_response.h
        zmq_event_subscription_change.h)


add_library(admin_commands OBJECT ${SOURCES} ${HEADERS})
target_compile_options(admin_commands PRIVATE -fPIC)
