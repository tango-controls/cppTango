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

add_executable(devTest ${SOURCES})
target_include_directories(devTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(devTest PUBLIC ${CMAKE_DL_LIBS} tango)
