set(SOURCES jpeg_bitstream.cpp
            jpeg_color.cpp
            jpeg_dct.cpp
            jpeg_decoder.cpp
            jpeg_encoder.cpp
            jpeg_memory.cpp)
IF(WIN32)
add_library(jpeg_objects_dyn OBJECT ${SOURCES})
add_library(jpeg_objects_sta OBJECT ${SOURCES})
target_compile_definitions(jpeg_objects_dyn PRIVATE "${dyn_defs}")
target_compile_definitions(jpeg_objects_sta PRIVATE "${static_defs}")
ELSE(WIN32)
add_library(jpeg_objects OBJECT ${SOURCES})
target_compile_options(jpeg_objects PRIVATE -fPIC)
ENDIF(WIN32)
