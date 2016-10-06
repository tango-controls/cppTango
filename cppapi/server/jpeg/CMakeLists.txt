set(SOURCES jpeg_bitstream.cpp
            jpeg_color.cpp
            jpeg_dct.cpp
            jpeg_decoder.cpp
            jpeg_encoder.cpp
            jpeg_memory.cpp)

set(HEADERS jpeg_bitstream.h
            jpeg_const.h
            jpeg_lib.h
            jpeg_memory.h)

add_library(jpeg_objects OBJECT ${SOURCES} ${HEADERS})
target_compile_options(jpeg_objects PRIVATE -fPIC)
target_include_directories(jpeg_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
