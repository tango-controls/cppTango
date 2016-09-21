set(SOURCES ClassFactory.cpp
            FwdTestClass.cpp
            FwdTest.cpp
            FwdTestStateMachine.cpp
            main.cpp)

add_executable(FwdTest ${SOURCES})
target_include_directories(FwdTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(FwdTest PUBLIC tango ${CMAKE_DL_LIBS})
