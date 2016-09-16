set(HELPERS DeviceProxyHelper.h
            PogoHelper.h
            TangoExceptionsHelper.h
            Xstring.h)

install(FILES ${HELPERS} DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")