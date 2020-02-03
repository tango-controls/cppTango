include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../jpeg)

set(SOURCES
        jpeg_color_mmx.cpp
        jpeg_dct_mmx.cpp)

if(WIN32)
    add_library(jpeg_mmx_objects_dyn OBJECT ${SOURCES})
    add_library(jpeg_mmx_objects_sta OBJECT ${SOURCES})
    target_compile_definitions(jpeg_mmx_objects_dyn PRIVATE "${dyn_defs}")
    target_compile_definitions(jpeg_mmx_objects_sta PRIVATE "${static_defs}")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(jpeg_mmx_objects_sta PRIVATE "/MTd")
    else()
        target_compile_options(jpeg_mmx_objects_sta PRIVATE "/MT")
    endif()
    if(${PLATFORM} EQUAL 64)
        target_compile_definitions(jpeg_mmx_objects_dyn PRIVATE _64BITS)
        target_compile_definitions(jpeg_mmx_objects_sta PRIVATE _64BITS)
    else()
        target_compile_options(jpeg_mmx_objects_dyn PRIVATE -O0)
	target_compile_options(jpeg_mmx_objects_sta PRIVATE -O0)
	if(TANGO_JPEG_MMX)
            target_compile_definitions(jpeg_mmx_objects_dyn PRIVATE JPG_USE_ASM)
            target_compile_definitions(jpeg_mmx_objects_sta PRIVATE JPG_USE_ASM)
	endif()
    endif()

else(WIN32)

    add_library(jpeg_mmx_objects OBJECT ${SOURCES})
    if(TANGO_JPEG_MMX)
        target_compile_options(jpeg_mmx_objects PRIVATE -mmmx -fPIC)
    else()
	target_compile_options(jpeg_mmx_objects PRIVATE -fPIC)
    endif()

    if(${PLATFORM} EQUAL 64)
        target_compile_definitions(jpeg_mmx_objects PRIVATE _64BITS)
    else()
        target_compile_options(jpeg_mmx_objects PRIVATE -O0)
	if(TANGO_JPEG_MMX)
            target_compile_definitions(jpeg_mmx_objects PRIVATE JPG_USE_ASM)
	endif()
    endif()

endif(WIN32)
