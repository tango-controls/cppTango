add_subdirectory(log4tango)
add_subdirectory(tango/client)
add_subdirectory(tango/server)

add_library(include_objects OBJECT tango.h $<TARGET_OBJECTS:include_log4tango_objects> $<TARGET_OBJECTS:include_client_objects> $<TARGET_OBJECTS:include_server_objects>)