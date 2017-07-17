find_package(Threads)
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
target_link_libraries(DevTest PUBLIC tango -fsanitize=thread -pie ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
target_compile_options(DevTest PUBLIC -fPIE -fsanitize=thread -g)
