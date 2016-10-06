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
        zmqeventsupplier.cpp)


add_subdirectory(jpeg)
add_subdirectory(jpeg_mmx)

add_library(server_objects OBJECT ${SOURCES})
target_compile_options(server_objects PRIVATE -fPIC)
target_include_directories(server_objects PRIVATE .)

