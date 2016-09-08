SUBDIRS=idl jpeg jpeg_mmx .

# We need the ORB to compile and the tango header files to compile
AM_CPPFLAGS = -I$(top_srcdir)/lib/cpp/client \
              -I$(top_srcdir)/lib/cpp/log4tango/include \
              -I$(top_builddir)/lib/cpp/log4tango/include \
              -I$(top_builddir)/lib/cpp/server \
              $(ORB_INCLUDE_PREFIX) $(LIBZMQ_CFLAGS)

# We're making a libtool convenience library which is not to be installed,
# therefore the automake noinst variable
noinst_LTLIBRARIES=libserver.la

AM_CXXFLAGS=-D_TANGO_LIB

# The sources
libserver_la_SOURCES = attrdesc.cpp                 \
                      attrgetsetprop.cpp            \
                      attribute.cpp                 \
                      attrsetval.cpp                \
                      attrmanip.cpp                 \
                      basiccommand.cpp              \
                      blackbox.cpp                  \
                      class_factory.cpp             \
                      classattribute.cpp            \
                      command.cpp                   \
                      coutappender.cpp              \
                      classpipe.cpp                 \
                      dev_event.cpp                 \
                      dev_poll.cpp                  \
                      device.cpp                    \
                      device_2.cpp                  \
                      device_3.cpp                  \
                      device_4.cpp                  \
                      device_5.cpp                  \
                      deviceclass.cpp               \
                      devicelog.cpp                 \
                      devintr.cpp                   \
                      dintrthread.cpp               \
                      dserver.cpp                   \
                      dserverclass.cpp              \
                      dserverlock.cpp               \
                      dserverlog.cpp                \
                      dserverpoll.cpp               \
                      dserversignal.cpp             \
                      encoded_attribute.cpp         \
                      eventcmds.cpp                 \
                      eventsupplier.cpp             \
                      except.cpp                    \
                      fwdattrdesc.cpp               \
                      fwdattribute.cpp              \
                      logcmds.cpp                   \
                      logging.cpp                   \
                      logstream.cpp                 \
                      multiattribute.cpp            \
                      notifdeventsupplier.cpp       \
                      pipe.cpp                      \
                      pollcmds.cpp                  \
                      pollobj.cpp                   \
                      pollring.cpp                  \
                      pollthread.cpp                \
                      rootattreg.cpp                \
                      seqvec.cpp                    \
                      subdev_diag.cpp               \
                      tangoappender.cpp             \
                      tangorollingfileappender.cpp  \
                      templ_inst.cpp                \
                      thsig.cpp                     \
                      utils.cpp                     \
                      utils_polling.cpp             \
                      utils_shut.cpp                \
                      w_attribute.cpp               \
                      w_pipe.cpp                    \
                      zmqeventsupplier.cpp


tangoincludedir = $(includedir)/tango

tangoinclude_HEADERS = attrdesc.h                 \
                       attribute.h                \
                       attrmanip.h                \
                       attrprop.h                 \
                       auto_tango_monitor.h       \
                       basiccommand.h             \
                       blackbox.h                 \
                       classattribute.h           \
                       classpipe.h                \
                       command.h                  \
                       coutappender.h             \
                       coutbuf.h                  \
                       device.h                   \
                       device_2.h                 \
                       device_3.h                 \
                       device_4.h                 \
                       device_5.h                 \
                       deviceclass.h              \
                       devintr.h                  \
                       dintrthread.h              \
                       dserver.h                  \
                       dserverclass.h             \
                       dserversignal.h            \
                       encoded_attribute.h        \
                       encoded_format.h           \
                       eventsupplier.h            \
                       except.h                   \
                       fwdattrdesc.h              \
                       fwdattribute.h             \
                       log4tango.h                \
                       logcmds.h                  \
                       logging.h                  \
                       logstream.h                \
                       multiattribute.h           \
                       ntservice.h                \
                       pipe.h                     \
                       pipedesc.h                 \
                       pollcmds.h                 \
                       pollext.h                  \
                       pollobj.h                  \
                       pollring.h                 \
                       pollthread.h               \
                       readers_writers_lock.h     \
                       rootattreg.h               \
                       seqvec.h                   \
                       subdev_diag.h              \
                       tango.h                    \
                       tango_config.h             \
                       tango_const.h              \
                       tango_monitor.h            \
                       tangoappender.h            \
                       tangorollingfileappender.h \
                       utils.h                    \
                       w_attribute.h              \
                       w_pipe.h                   \
                       attribute.tpp              \
                       attribute_spec.tpp         \
                       attrprop.tpp               \
                       attrsetval.tpp             \
                       device_3.tpp               \
                       fwdattribute.tpp           \
                       fwdattribute_spec.tpp      \
                       pipe.tpp                   \
                       pollext.tpp                \
                       pollring.tpp               \
                       pollthread.tpp             \
                       utils.tpp                  \
                       utils_spec.tpp             \
                       w_attribute.tpp            \
                       w_attribute_spec.tpp       \
                       w_attrsetval.tpp           \
                       w_pipe.tpp
