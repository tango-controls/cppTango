set(SOURCES)
set(HEADERS event_subscription.h)


add_library(event_objects OBJECT ${SOURCES} ${HEADERS})
target_compile_options(event_objects PRIVATE -fPIC)