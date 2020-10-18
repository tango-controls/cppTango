set(SOURCES attrdesc.cpp
            attrgetsetprop.cpp
            attribute.cpp
            attrsetval.cpp
            attrmanip.cpp
            basiccommand.cpp
            blackbox.cpp
            class_factory.cpp
            classattribute.cpp
            command.cpp
            coutappender.cpp
            classpipe.cpp
            dev_event.cpp
            dev_poll.cpp
            device.cpp
            device_2.cpp
            device_3.cpp
            device_4.cpp
            device_5.cpp
            deviceclass.cpp
            devicelog.cpp
            devintr.cpp
            dintrthread.cpp
            dserver.cpp
            dserverclass.cpp
            dserverlock.cpp
            dserverlog.cpp
            dserverpoll.cpp
            dserversignal.cpp
            encoded_attribute.cpp
            eventcmds.cpp
            eventsupplier.cpp
            except.cpp
            fwdattrdesc.cpp
            fwdattribute.cpp
            logcmds.cpp
            logging.cpp
            logstream.cpp
            multiattribute.cpp
            notifdeventsupplier.cpp
            pipe.cpp
            pollcmds.cpp
            pollobj.cpp
            pollring.cpp
            pollthread.cpp
            rootattreg.cpp
            seqvec.cpp
            subdev_diag.cpp
            tangoappender.cpp
            tangorollingfileappender.cpp
            templ_inst.cpp
            thsig.cpp
            utils.cpp
            utils_polling.cpp
            utils_shut.cpp
            w_attribute.cpp
            w_pipe.cpp
            public_attribute.cpp
            public_w_attribute.cpp
            zmqeventsupplier.cpp)

set(HEADERS attrdesc.h
            attribute.h
            attribute.tpp
            attrsetval.tpp
            attribute_spec.tpp
            attrmanip.h
            attrprop.h
            attrprop.tpp
            auto_tango_monitor.h
            basiccommand.h
            blackbox.h
            classattribute.h
            classpipe.h
            command.h
            pipe.h
            pipe.tpp
            coutappender.h
            coutbuf.h
            device.h
            device_2.h
            device_3.h
            device_3.tpp
            device_4.h
            device_5.h
            deviceclass.h
            devintr.h
            dintrthread.h
            dserver.h
            dserverclass.h
            dserversignal.h
            eventsupplier.h
            except.h
            fwdattrdesc.h
            fwdattribute.h
            fwdattribute.tpp
            fwdattribute_spec.tpp
            log4tango.h
            logcmds.h
            logging.h
            logstream.h
            multiattribute.h
            ntservice.h
            pipedesc.h
            pollcmds.h
            pollext.h
            pollext.tpp
            pollobj.h
            pollring.h
            pollring.tpp
            pollthread.h
            pollthread.tpp
            readers_writers_lock.h
            rootattreg.h
            seqvec.h
            tango.h
            tango_config.h
            tango_monitor.h
            tangoappender.h
            tangorollingfileappender.h
            utils.h
            utils.tpp
            utils_spec.tpp
            w32win.h
            w_attribute.h
            w_attribute.tpp
            w_attrsetval.tpp
            w_attribute_spec.tpp
            w_pipe.h
            w_pipe.tpp
            subdev_diag.h
            encoded_attribute.h
            encoded_format.h
            event_subscription_state.h)

add_subdirectory(idl)
add_subdirectory(jpeg)
add_subdirectory(jpeg_mmx)

if(WIN32)
    set(SOURCES_WIN
        coutbuf.cpp
        ntservice.cpp
        w32win.cpp)

    list(APPEND SOURCES ${SOURCES_WIN})
endif()

add_library(server_objects OBJECT ${SOURCES})

if(WIN32)
    target_compile_definitions(server_objects PRIVATE "${windows_defs}")

    if(USE_PCH)
        tango_target_pch(server_objects)
    endif()
else()
    target_compile_options(server_objects PRIVATE -fPIC)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(server_objects PRIVATE _REENTRANT _TANGO_LIB)
    else()
        # Do not define _TANGO_LIB when compiling Tango debug library on Linux
        # in order to keep the same behaviour as in the past:
        # A Linux Tango Device Server using the debug tango lib will display
        # twice "Ready to accept requests" at startup
        target_compile_definitions(server_objects PRIVATE _REENTRANT)
    endif()
    if(USE_PCH)
        tango_add_pch(tango_pch server_objects "")
        tango_target_pch(server_objects tango_pch)
    endif()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tango_const.h.in ${CMAKE_CURRENT_BINARY_DIR}/tango_const.h)

install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tango_const.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango")
