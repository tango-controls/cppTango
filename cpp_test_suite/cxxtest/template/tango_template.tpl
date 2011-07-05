// -*- C++ -*-
// Tango main template
//

#define CXXTEST_HAVE_EH
#define CXXTEST_HAVE_STD
#define CXXTEST_ABORT_TEST_ON_FAIL
#include <cxxtest/TangoPrinter.h>

int main( int argc, char **argv )
{
    return CxxTest::TangoPrinter().run( argc, argv );
}

// The CxxTest "world"
<CxxTest world>