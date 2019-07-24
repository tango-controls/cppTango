
function(tango_target_pch target)

    set(tango_pch_file "${CMAKE_CURRENT_BINARY_DIR}/pch_${target}.pch")
    set(tango_cpp_file "${CMAKE_CURRENT_BINARY_DIR}/pch_${target}.cpp")

    add_custom_command(
        OUTPUT "${tango_cpp_file}"
        COMMAND cmake -E touch "${tango_cpp_file}")

    target_sources(${target} PRIVATE "${tango_cpp_file}")

    target_compile_options(${target} PRIVATE
        "/FItango.h"
        "/Yutango.h"
        "/Fp${tango_pch_file}")

    set_source_files_properties(
        "${tango_cpp_file}" PROPERTIES
        COMPILE_OPTIONS "/Yl-;/Yctango.h")

endfunction()
