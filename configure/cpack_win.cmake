
  IF(MSVC90)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      IF(CMAKE_CL_64)
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x64_debug)
      ELSE()
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x86_debug)
      ENDIF()
    else()
      IF(CMAKE_CL_64)
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x64)
      ELSE()
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x86)
      ENDIF()
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  ELSE()
    IF(CMAKE_CL_64)
      if (CMAKE_BUILD_TYPE STREQUAL "Debug")  
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x64_debug)
      else()
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x64)
      endif()
    ELSE()
      if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x86_debug)
      else()
        set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x86)
      endif()
    ENDIF()
  ENDIF(MSVC90)
#
# CPACK macros below here
#
set (CPACK_PACKAGE_NAME "Tango")
set (CPACK_PACKAGE_VENDOR "www.tango-controls.org")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tango - Connecting Things Together")
set (CPACK_PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set (CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
set (CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
set (CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})

# Define components and their display names
set (CPACK_COMPONENTS_ALL static dynamic headers)

# Human readable component descriptions
set (CPACK_COMPONENT_STATIC_DISPLAY_NAME
  "Tango.lib")
set (CPACK_COMPONENT_DYNAMIC_DISPLAY_NAME
  "Tango.dll")
set (CPACK_COMPONENT_HEADERS_DISPLAY_NAME
  "c++ Headers")
  set(CPACK_COMPONENT_STATIC_DESCRIPTION 
   "Tango.lib - for building projects")
 set(CPACK_COMPONENT_DYNAMIC_DESCRIPTION
   "Tango.dll - for running Tango executables")
 set(CPACK_COMPONENT_HEADERS_DESCRIPTION
   "C/C++ header files for use with Tango")

# Define NSIS installation types
set(CPACK_ALL_INSTALL_TYPES Development Runtime)
set(CPACK_COMPONENT_STATIC_GROUP "Development" )
set(CPACK_COMPONENT_DYNAMIC_GROUP "Runtime")
set(CPACK_COMPONENT_HEADERS_GROUP "Development")

set(CPACK_COMPONENT_STATIC_INSTALL_TYPES Development)
set(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Development)
set(CPACK_COMPONENT_DYNAMIC_INSTALL_TYPES Development Runtime)

set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION
   "All of the tools you'll ever need to develop Tango software")
set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION
   "All of the required libraries to launch Tango software")

set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_BINARY_DIR}/LICENSE)
set(CPACK_NSIS_HELP_LINK "http://www.tango-controls.org")
set(CPACK_NSIS_URL_INFO_ABOUT "http://www.tango-controls.org")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MENU_LINKS
    "http://tango-controls.readthedocs.io/en/latest/" "Tango Doc")
set(CPACK_NSIS_MUI_ICON ${PROJECT_BINARY_DIR}/appveyor/tango.ico)
set(CPACK_NSIS_MUI_UNIICON ${PROJECT_BINARY_DIR}/appveyor/tango.ico)
