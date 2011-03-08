

# We need the ORB to compile and the tango header files to compile
INCLUDES = -I$(top_srcdir)/lib/cpp/server \
           -I$(top_srcdir)/lib/cpp/log4tango/include \
	       -I$(top_builddir)/lib/cpp/log4tango/include \
           $(ORB_INCLUDE_PREFIX)

# We're making a libtool convenience library which is not to be installed,
# therefore the automake noinst variable
noinst_LTLIBRARIES = libjpeg.la

AM_CXXFLAGS=@JPEG_LIB_CXXFLAGS@

# These are the sources for the library.
libjpeg_la_SOURCES =	jpeg_bitstream.cpp	\
			  			jpeg_color.cpp  	\
		  	  			jpeg_dct.cpp		\
			  			jpeg_decoder.cpp	\
			  			jpeg_encoder.cpp	\
			  			jpeg_memory.cpp		\
						jpeg_bitstream.h	\
				  		jpeg_const.h 		\
		  		  		jpeg_lib.h  		\
		  		  		jpeg_memory.h
						
