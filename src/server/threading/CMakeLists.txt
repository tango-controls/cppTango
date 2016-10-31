set(SOURCES
        asymmetric_unbounded_blocking_queue.hxx
        asymmetric_unbounded_blocking_queue.cxx
        repeated_task.hxx
        repeated_task.cxx
        enhanced_thread.hxx
        enhanced_thread.cxx)

add_library(threading_objects OBJECT ${SOURCES})
target_include_directories(threading_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(threading_objects PRIVATE -fPIC)
