set(SOURCES)
set(HEADERS zmq_event_subscription_change_response.h)


add_library(admin_commands OBJECT ${SOURCES} ${HEADERS})
target_compile_options(admin_commands PRIVATE -fPIC)
