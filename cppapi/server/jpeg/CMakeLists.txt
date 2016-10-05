set(SOURCES jpeg_bitstream.cpp
            jpeg_color.cpp
            jpeg_dct.cpp
            jpeg_decoder.cpp
            jpeg_encoder.cpp
            jpeg_memory.cpp)

add_library(jpeg_objects OBJECT ${SOURCES})
target_compile_options(jpeg_objects PRIVATE -fPIC)
