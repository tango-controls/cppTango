set(SOURCES)
set(HEADERS utils.h)

add_subdirectory(admin/commands)
add_subdirectory(event)

add_library(common_objects OBJECT ${SOURCES} ${HEADERS})
target_compile_options(common_objects PRIVATE -fPIC)