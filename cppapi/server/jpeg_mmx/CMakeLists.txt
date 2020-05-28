include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../jpeg)

set(SOURCES
        jpeg_color_mmx.cpp
        jpeg_dct_mmx.cpp)

add_library(jpeg_mmx_objects OBJECT ${SOURCES})

if(WIN32)
    target_compile_definitions(jpeg_mmx_objects PRIVATE "${windows_defs}")
else()
    if(TANGO_JPEG_MMX)
        target_compile_options(jpeg_mmx_objects PRIVATE -mmmx -fPIC)
    else()
        target_compile_options(jpeg_mmx_objects PRIVATE -fPIC)
    endif()
endif()

if(${PLATFORM} EQUAL 64)
    target_compile_definitions(jpeg_mmx_objects PRIVATE _64BITS)
else()
    target_compile_options(jpeg_mmx_objects PRIVATE -O0)
    if(TANGO_JPEG_MMX)
        target_compile_definitions(jpeg_mmx_objects PRIVATE JPG_USE_ASM)
    endif()
endif()
