set(SOURCES ClassFactory.cpp
        FwdTestClass.cpp
        FwdTest.cpp
        FwdTestStateMachine.cpp
        main.cpp)

add_executable(FwdTest ${SOURCES})
target_include_directories(FwdTest PUBLIC .)
target_link_libraries(FwdTest PUBLIC tango)
