set(SOURCES
        jpeg_color_mmx.cpp
        jpeg_dct_mmx.cpp)

add_library(jpeg_mmx_objects OBJECT ${SOURCES})
target_compile_options(jpeg_mmx_objects PRIVATE -mmmx -fPIC)
target_include_directories(jpeg_mmx_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../jpeg)
if(${PLATFORM} EQUAL 64)
    target_compile_definitions(jpeg_mmx_objects PRIVATE _64BITS)
else()
    target_compile_options(jpeg_mmx_objects PRIVATE -O0)
    target_compile_definitions(jpeg_mmx_objects PRIVATE JPG_USE_ASM)
endif()
