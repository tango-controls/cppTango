include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../jpeg)

set(SOURCES
        jpeg_color_mmx.cpp
        jpeg_dct_mmx.cpp)

IF(WIN32)
add_library(jpeg_mmx_objects_dyn OBJECT ${SOURCES})
add_library(jpeg_mmx_objects_sta OBJECT ${SOURCES})
target_compile_options(jpeg_mmx_objects_dyn PRIVATE "${dyn_defs}")
target_compile_options(jpeg_mmx_objects_sta PRIVATE "${static_defs}")

if(${PLATFORM} EQUAL 64)
    target_compile_definitions(jpeg_mmx_objects_dyn PRIVATE _64BITS)
    target_compile_definitions(jpeg_mmx_objects_sta PRIVATE _64BITS)
else()
    target_compile_options(jpeg_mmx_objects_dyn PRIVATE -O0)
    target_compile_definitions(jpeg_mmx_objects_dyn PRIVATE JPG_USE_ASM)
    target_compile_options(jpeg_mmx_objects_sta PRIVATE -O0)
    target_compile_definitions(jpeg_mmx_objects_sta PRIVATE JPG_USE_ASM)
endif()

ELSE(WIN32)

add_library(jpeg_mmx_objects OBJECT ${SOURCES})
target_compile_options(jpeg_mmx_objects PRIVATE -mmmx -fPIC)

if(${PLATFORM} EQUAL 64)
    target_compile_definitions(jpeg_mmx_objects PRIVATE _64BITS)
else()
    target_compile_options(jpeg_mmx_objects PRIVATE -O0)
    target_compile_definitions(jpeg_mmx_objects PRIVATE JPG_USE_ASM)
endif()

ENDIF(WIN32)
