set(SOURCES main.cpp
        TypeCmds.cpp
        SigThrow.cpp
        IOMisc.cpp
        IOStr1.cpp
        IOStr2.cpp
        IOArray1.cpp
        IOArray2.cpp
        IOStrArray.cpp
        IOStruct.cpp
        IOSetAttr.cpp
        IOAddAttribute.cpp
        IOSeqVec.cpp
        FileDb.cpp
        classfactory.cpp
        DevTestClass.cpp
        DevTest.cpp)

add_subdirectory(fwd_ds)

add_executable(DevTest ${SOURCES})
target_include_directories(DevTest PUBLIC .)
target_link_libraries(DevTest PUBLIC tango ${CMAKE_DL_LIBS})
if(USE_PCH)
    tango_target_test_pch(DevTest)
    target_compile_definitions(DevTest PRIVATE ${COMMON_TEST_CXX_DEFS})
endif()
