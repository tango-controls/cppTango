#ifndef CXXCommon_H
#define CXXCommon_H

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <iostream>
#include <tango.h>

using namespace std;
using namespace Tango;

#undef cout
#define cout std::cout << "\t"

#endif // CXXCommon_H
