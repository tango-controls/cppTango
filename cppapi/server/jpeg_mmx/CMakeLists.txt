include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../jpeg)

set(SOURCES
        jpeg_color_mmx.cpp
        jpeg_dct_mmx.cpp)

add_library(jpeg_mmx_objects OBJECT ${SOURCES})

if(CMAKE_VERSION VERSION_GREATER 2.8.12)
    target_compile_options(jpeg_mmx_objects PRIVATE -mmmx)
else()
    set_property(TARGET jpeg_mmx_objects APPEND PROPERTY COMPILE_OPTIONS "-mmx")
endif()

if(${PLATFORM} EQUAL 64)
    if(CMAKE_VERSION VERSION_GREATER 2.8.11)
        target_compile_definitions(jpeg_mmx_objects PRIVATE _64BITS)
    else()
        set_property(TARGET jpeg_mmx_objects APPEND PROPERTY COMPILE_DEFINITIONS _64BITS)
	endif()
else()
    if(CMAKE_VERSION VERSION_GREATER 2.8.12)
        target_compile_options(jpeg_mmx_objects PRIVATE -O0)
    else()
        set_property(TARGET jpeg_mmx_objects APPEND PROPERTY COMPILE_OPTIONS "-O0")
    endif()
    if(CMAKE_VERSION VERSION_GREATER 2.8.11)
        target_compile_definitions(jpeg_mmx_objects PRIVATE JPG_USE_ASM)
    else()
        set_property(TARGET jpeg_mmx_objects APPEND PROPERTY COMPILE_DEFINITIONS JPG_USE_ASM)
    endif()
endif()
