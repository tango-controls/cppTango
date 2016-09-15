set(SOURCES classfactory.cpp
            DevTest.cpp
            IOAddAttribute.cpp
            IOArray2.cpp
            IOMisc.cpp
            IOSetAttr.cpp
            IOStr2.cpp
            IOStruct.cpp
#            main_win.cpp
            TypeCmds.cpp
            DevTestClass.cpp
            FileDb.cpp
            IOArray1.cpp
            IOLongVar.cpp
            IOSeqVec.cpp
            IOStr1.cpp
            IOStrArray.cpp
            main.cpp
            SigThrow.cpp)

add_subdirectory(fwd_ds)

add_executable(devTest ${SOURCES})
target_include_directories(devTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(devTest PUBLIC ${CMAKE_DL_LIBS} tango)
