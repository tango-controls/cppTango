set(SOURCES ClassFactory.cpp
            FwdTestClass.cpp
            FwdTest.cpp
            FwdTestStateMachine.cpp
            main.cpp)

add_executable(FwdTest ${SOURCES})
if(CMAKE_VERSION VERSION_GREATER 2.8.12)
    target_include_directories(FwdTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(FwdTest PUBLIC tango ${CMAKE_DL_LIBS})
else()
    set_property(TARGET FwdTest APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(FwdTest LINK_PUBLIC tango ${CMAKE_DL_LIBS})
endif()
