set(SOURCES ClassFactory.cpp
        FwdTestClass.cpp
        FwdTest.cpp
        FwdTestStateMachine.cpp
        main.cpp)

add_executable(FwdTest ${SOURCES})
target_include_directories(FwdTest PUBLIC .)
target_compile_options(FwdTest PUBLIC -fsanitize=thread -g)
target_link_libraries(FwdTest PUBLIC -fsanitize=thread -no-pie tango ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
