#definitions and preprocessor
add_definitions(-D__x86__)
add_definitions(-D__POSIX_NT__)
add_definitions(-D__OSVERSION__=4)
add_definitions(-DOMNI_UNLOADABLE_STUBS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DWIN32)
add_definitions(-DHAVE_STRUCT_TIMESPEC)
add_definitions(-D_TIMESPEC_DEFINED)
add_definitions(-DSTRUCT_TIMESPEC_HAS_TV_SEC)
add_definitions(-DSTRUCT_TIMESPEC_HAS_TV_NSEC)
