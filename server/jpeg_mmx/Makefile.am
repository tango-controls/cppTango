

# We need the ORB to compile and the tango header files to compile
INCLUDES = -I$(top_srcdir)/lib/cpp/server \
           -I$(top_srcdir)/lib/cpp/log4tango/include \
	   	   -I$(top_builddir)/lib/cpp/log4tango/include \
		   -I$(top_srcdir)/lib/cpp/server/jpeg \
           $(ORB_INCLUDE_PREFIX)

# We're making a libtool convenience library which is not to be installed,
# therefore the automake noinst variable
noinst_LTLIBRARIES = libjpeg_mmx.la
						
libjpeg_mmx_la_CXXFLAGS=@JPEG_MMX_LIB_CXXFLAGS@ -O0

# These are the sources for the library.
libjpeg_mmx_la_SOURCES =	jpeg_color_mmx.cpp 	\
			  				jpeg_dct_mmx.cpp
					   
