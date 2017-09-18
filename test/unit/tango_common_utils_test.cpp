//===================================================================================================================
//
// file :		tango_common_utils_test.cpp
//
// project :		tango
//
// author(s) :		ingvord
//
// Copyright (C) :      2004-2017
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//===================================================================================================================

#include <cassert>
#include "../../src/common/utils.h"

int main(int argc, char **argv)
{
    assert("hello, world!!!" == tango::common::StringUtils::to_lower_case("hEllO, WorLd!!!"));


    std::string input{"aBc"};
    assert("abc" == tango::common::StringUtils::to_lower_case(input));
}