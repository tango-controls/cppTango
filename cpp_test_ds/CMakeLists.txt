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
if(CMAKE_VERSION VERSION_GREATER 2.8.11)
    target_include_directories(DevTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
else()
    set_property(TARGET DevTest APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})
endif()
if(CMAKE_VERSION VERSION_GREATER 2.8.12)
    target_link_libraries(DevTest PUBLIC tango ${CMAKE_DL_LIBS})
else()
    target_link_libraries(DevTest LINK_PUBLIC tango ${CMAKE_DL_LIBS})
endif()
