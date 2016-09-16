include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../jpeg)

set(SOURCES
        jpeg_color_mmx.cpp
        jpeg_dct_mmx.cpp)

add_library(jpeg_mmx_objects OBJECT ${SOURCES})
