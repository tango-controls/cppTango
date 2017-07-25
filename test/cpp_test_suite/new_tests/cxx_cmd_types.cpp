//===================================================================================================================
//
// file :		cxx_cmd_types.cpp
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
//
// Created by ingvord on 3/13/17.
//
#ifndef CmdTypesTestSuite_h
#define CmdTypesTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <thread>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME CmdTypesTestSuite

class CmdTypesTestSuite: public CxxTest::TestSuite
{
protected:
    DeviceProxy *device;
    size_t loop = 10;

public:
    SUITE_NAME()
    {

//
// Arguments check -------------------------------------------------
//

        string device_name;

        device_name = CxxTest::TangoPrinter::get_param("device1");

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try
        {
            device = new DeviceProxy(device_name);
            device->ping();
        }
        catch (CORBA::Exception &e)
        {
            Except::print_exception(e);
            exit(-1);
        }

    }

    virtual ~SUITE_NAME()
    {
        delete device;
    }

    static SUITE_NAME *createSuite()
    {
        return new SUITE_NAME();
    }

    static void destroySuite(SUITE_NAME *suite)
    {
        delete suite;
    }

//
// Tests -------------------------------------------------------
//

    void test_void(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            TS_ASSERT_THROWS_NOTHING(device->command_inout("IOVoid"));
        }
    }

    void test_boolean(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            bool in = true;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOBool", din));
            bool received;
            int data_type = dout.get_type();
            dout >> received;
            TS_ASSERT_DIFFERS(in, received);
            TS_ASSERT_EQUALS(data_type, Tango::DEV_BOOLEAN);
        }
    }

    void test_short(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            short in = 2;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOShort", din));
            short received;
            dout >> received;
            int data_type = dout.get_type();
            TS_ASSERT_EQUALS(received, (in * 2));
            TS_ASSERT_EQUALS(data_type, Tango::DEV_SHORT);
        }
    }

    void test_long(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevLong in = 3;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOLong", din));
            DevLong received;
            dout >> received;
            TS_ASSERT_EQUALS(received, (in * 2));
        }
    }

    void test_float()
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            float in = (float) 3.1;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOFloat", din));
            float received;
            dout >> received;
            TS_ASSERT_EQUALS(received, (in * 2));
        }
    }

    void test_double(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            double in = 3.1;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IODouble", din));
            double received;
            dout >> received;
            TS_ASSERT_EQUALS(received, (in * 2));
        }
    }

    void test_unsigned_short(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            unsigned short in = 100;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOUShort", din));
            unsigned short received;
            dout >> received;
            TS_ASSERT_EQUALS(received, (in * 2));
        }
    }

    void test_unsigned_long(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevULong in = 1000;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOULong", din));
            DevULong received;
            dout >> received;
            TS_ASSERT_EQUALS(received, (in * 2));
        }
    }

    void test_CPP_string(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            string str("abc");
            din << str;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOString", din));
            string received;
            dout >> received;
            TS_ASSERT_EQUALS(received, "cba");
        }
    }

    void test_classical_C_string(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            const char *str = "abcd";
            din << str;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOString", din));
            const char *received;
            dout >> received;
            TS_ASSERT(strcmp(str, "dcba"));
        }
    }

    void test_char_array(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<unsigned char> in;
            in.push_back(1);
            in.push_back(2);
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOCharArray", din));
            vector<unsigned char> received;
            dout >> received;
            int data_type = dout.get_type();
            TS_ASSERT_EQUALS(in[0], received[1]);
            TS_ASSERT_EQUALS(in[1], received[0]);
            TS_ASSERT_EQUALS(data_type, Tango::DEVVAR_CHARARRAY);
        }
    }

    void test_DevVarCharArray(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            const DevVarCharArray *received;
            DeviceData din, dout;

            DevVarCharArray *in = new DevVarCharArray(2);
            in->length(2);
            (*in)[0] = 10;
            (*in)[1] = 20;
            din << in;

            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOCharArray", din));
            dout >> received;

            TS_ASSERT_EQUALS(10, (*received)[1]);
            TS_ASSERT_EQUALS(20, (*received)[0]);
        }

        for (size_t i = 0; i < loop; i++)
        {
            const DevVarCharArray *received;
            DeviceData din, dout;

            DevVarCharArray in(2);
            in.length(2);
            in[0] = 10;
            in[1] = 20;
            din << in;

            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOCharArray", din));
            dout >> received;

            TS_ASSERT_EQUALS(in[0], (*received)[1]);
            TS_ASSERT_EQUALS(in[1], (*received)[0]);
        }
    }

    void test_short_array(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<short> in;
            in.push_back(10);
            in.push_back(20);
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOShortArray", din));
            vector<short> received;
            dout >> received;
            TS_ASSERT_EQUALS(received[0], (in[0] * 2));
            TS_ASSERT_EQUALS(received[1], (in[1] * 2));
        }
    }

    void test_DevVarShortArray(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarShortArray *in = new DevVarShortArray(2);
            in->length(2);
            (*in)[0] = 1;
            (*in)[1] = 2;

            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOShortArray", din));
            const DevVarShortArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (1 * 2));
            TS_ASSERT_EQUALS((*received)[1], (2 * 2));
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarShortArray in(2);
            in.length(2);
            in[0] = 1;
            in[1] = 2;

            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOShortArray", din));
            const DevVarShortArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (in[0] * 2));
            TS_ASSERT_EQUALS((*received)[1], (in[1] * 2));
        }
    }

    void test_long_array(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<DevLong> in;
            in.push_back(100);
            in.push_back(200);
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOLongArray", din));
            vector<DevLong> received;
            dout >> received;
            TS_ASSERT_EQUALS(received[0], (in[0] * 2));
            TS_ASSERT_EQUALS(received[1], (in[1] * 2));
        }
    }

    void test_DevVarLongArray(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarLongArray *in = new DevVarLongArray(2);
            in->length(2);
            (*in)[0] = 11;
            (*in)[1] = 22;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOLongArray", din));
            const DevVarLongArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (11 * 2));
            TS_ASSERT_EQUALS((*received)[1], (22 * 2));
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarLongArray in(2);
            in.length(2);
            in[0] = 11;
            in[1] = 22;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOLongArray", din));
            const DevVarLongArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (in[0] * 2));
            TS_ASSERT_EQUALS((*received)[1], (in[1] * 2));
        }
    }

    void test_float_array(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<float> in;
            in.push_back((float) 100.1);
            in.push_back((float) 200.2);
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOFloatArray", din));
            vector<float> received;
            dout >> received;
            TS_ASSERT_EQUALS(received[0], (in[0] * 2));
            TS_ASSERT_EQUALS(received[1], (in[1] * 2));
        }
    }

    void test_DevVarFloatArray(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarFloatArray *in = new DevVarFloatArray(2);
            in->length(2);
            (*in)[0] = (float) 1.11;
            (*in)[1] = (float) 2.22;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOFloatArray", din));
            const DevVarFloatArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], ((float) 1.11 * 2));
            TS_ASSERT_EQUALS((*received)[1], ((float) 2.22 * 2));
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarFloatArray in(2);
            in.length(2);
            in[0] = (float) 1.11;
            in[1] = (float) 2.22;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOFloatArray", din));
            const DevVarFloatArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (in[0] * 2));
            TS_ASSERT_EQUALS((*received)[1], (in[1] * 2));
        }
    }

    void test_double_array(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<double> in;
            in.push_back(1.234);
            in.push_back(2.111);
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IODoubleArray", din));
            vector<double> received;
            dout >> received;
            TS_ASSERT_EQUALS(received[0], (in[0] * 2));
            TS_ASSERT_EQUALS(received[1], (in[1] * 2));
        }
    }

    void test_DevVarDoubleArray(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarDoubleArray *in = new DevVarDoubleArray(2);
            in->length(2);
            (*in)[0] = 1.12;
            (*in)[1] = 3.45;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IODoubleArray", din));
            const DevVarDoubleArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (1.12 * 2));
            TS_ASSERT_EQUALS((*received)[1], (3.45 * 2));
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarDoubleArray in(2);
            in.length(2);
            in[0] = 1.12;
            in[1] = 3.45;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IODoubleArray", din));
            const DevVarDoubleArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (in[0] * 2));
            TS_ASSERT_EQUALS((*received)[1], (in[1] * 2));
        }
    }

    void test_unsigned_short_array(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<unsigned short> in;
            in.push_back(100);
            in.push_back(200);
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOUShortArray", din));
            vector<unsigned short> received;
            dout >> received;
            TS_ASSERT_EQUALS(received[0], (in[0] * 2));
            TS_ASSERT_EQUALS(received[1], (in[1] * 2));
        }
    }

    void test_DevVarUShortArray(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarUShortArray *in = new DevVarUShortArray(2);
            in->length(2);
            (*in)[0] = 11;
            (*in)[1] = 22;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOUShortArray", din));
            const DevVarUShortArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (11 * 2));
            TS_ASSERT_EQUALS((*received)[1], (22 * 2));
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarUShortArray in(2);
            in.length(2);
            in[0] = 11;
            in[1] = 22;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOUShortArray", din));
            const DevVarUShortArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (in[0] * 2));
            TS_ASSERT_EQUALS((*received)[1], (in[1] * 2));
        }
    }

    void test_unsigned_long_array(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<DevULong> in;
            in.push_back(1000);
            in.push_back(2001);
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOULongArray", din));
            vector<DevULong> received;
            dout >> received;
            TS_ASSERT_EQUALS(received[0], (in[0] * 2));
            TS_ASSERT_EQUALS(received[1], (in[1] * 2));
        }
    }

    void test_DevVarULongArray(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarULongArray *in = new DevVarULongArray(2);
            in->length(2);
            (*in)[0] = 111;
            (*in)[1] = 222;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOULongArray", din));
            const DevVarULongArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (111 * 2));
            TS_ASSERT_EQUALS((*received)[1], (222 * 2));
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarULongArray in(2);
            in.length(2);
            in[0] = 111;
            in[1] = 222;
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOULongArray", din));
            const DevVarULongArray *received;
            dout >> received;
            TS_ASSERT_EQUALS((*received)[0], (in[0] * 2));
            TS_ASSERT_EQUALS((*received)[1], (in[1] * 2));
        }
    }

    void test_string_array(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<string> in;
            in.push_back("abc");
            in.push_back("wxyz");
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOStringArray", din));
            vector<string> received;
            int data_type = dout.get_type();
            dout >> received;

            TS_ASSERT_EQUALS(received[0], in[1]);
            TS_ASSERT_EQUALS(received[1], in[0]);
            TS_ASSERT_EQUALS(data_type, Tango::DEVVAR_STRINGARRAY);
        }
    }

    void test_DevVarStringArray(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarStringArray *in = new DevVarStringArray(2);
            in->length(2);
            (*in)[0] = Tango::string_dup("abc");
            (*in)[1] = Tango::string_dup("def");
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOStringArray", din));
            const DevVarStringArray *received;
            dout >> received;
            TS_ASSERT(not(strcmp((*received)[0], "def")));
            TS_ASSERT(not(strcmp((*received)[1], "abc")));
        }
    }

    void test_vector_of_long_and_vector_of_string(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<DevLong> in1;
            in1.push_back(1000);
            in1.push_back(2001);
            in1.push_back(2002);
            vector<string> in2;
            in2.push_back("abc");
            in2.push_back("def");
            din.insert(in1, in2);
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOLongString", din));
            vector<DevLong> received1;
            vector<string> received2;
            dout.extract(received1, received2);

            TS_ASSERT_EQUALS(received2[0], in2[0]);
            TS_ASSERT_EQUALS(received2[1], in2[1]);
            TS_ASSERT_EQUALS(received1[0], (in1[0] * 2));
            TS_ASSERT_EQUALS(received1[1], (in1[1] * 2));
            TS_ASSERT_EQUALS(received1[2], (in1[2] * 2));
        }
    }

    void test_DevVarLongStringArray(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarLongStringArray *in = new DevVarLongStringArray();
            in->lvalue.length(2);
            in->lvalue[0] = 111;
            in->lvalue[1] = 222;
            in->svalue.length(2);
            in->svalue[0] = Tango::string_dup("zxc");
            in->svalue[1] = Tango::string_dup("qwe");
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOLongString", din));
            const DevVarLongStringArray *received;
            dout >> received;

            TS_ASSERT_EQUALS(received->lvalue[0], (111 * 2));
            TS_ASSERT_EQUALS(received->lvalue[1], (222 * 2));
            TS_ASSERT(not(strcmp(received->svalue[0], "zxc")));
            TS_ASSERT(not(strcmp(received->svalue[1], "qwe")));
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarLongStringArray in;
            in.lvalue.length(2);
            in.lvalue[0] = 111;
            in.lvalue[1] = 222;
            in.svalue.length(2);
            in.svalue[0] = Tango::string_dup("zxc");
            in.svalue[1] = Tango::string_dup("qwe");
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOLongString", din));
            const DevVarLongStringArray *received;
            dout >> received;
            int data_type = dout.get_type();

            TS_ASSERT_EQUALS(received->lvalue[0], (in.lvalue[0] * 2));
            TS_ASSERT_EQUALS(received->lvalue[1], (in.lvalue[1] * 2));
            TS_ASSERT(not(strcmp(received->svalue[0], in.svalue[0])));
            TS_ASSERT(not(strcmp(received->svalue[1], in.svalue[1])));
            TS_ASSERT_EQUALS(data_type, Tango::DEVVAR_LONGSTRINGARRAY);
        }
    }

    void test_vector_of_double_and_vector_of_string(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            vector<double> in1;
            in1.push_back(10.0);
            in1.push_back(20.1);
            in1.push_back(20.2);
            vector<string> in2;
            in2.push_back("abc");
            in2.push_back("def");
            din.insert(in1, in2);
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IODoubleString", din));
            vector<double> received1;
            vector<string> received2;
            dout.extract(received1, received2);
            int data_type = dout.get_type();

            TS_ASSERT_EQUALS(received2[0], in2[0]);
            TS_ASSERT_EQUALS(received2[1], in2[1]);
            TS_ASSERT_EQUALS(received1[0], (in1[0] * 2));
            TS_ASSERT_EQUALS(received1[1], (in1[1] * 2));
            TS_ASSERT_EQUALS(received1[2], (in1[2] * 2));
            TS_ASSERT_EQUALS(data_type, Tango::DEVVAR_DOUBLESTRINGARRAY);
        }
    }

    void test_DevVarDoubleStringArray(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarDoubleStringArray *in = new DevVarDoubleStringArray();
            in->dvalue.length(2);
            in->dvalue[0] = 1.11;
            in->dvalue[1] = 22.2;
            in->svalue.length(3);
            in->svalue[0] = Tango::string_dup("iop");
            in->svalue[1] = Tango::string_dup("jkl");
            in->svalue[2] = Tango::string_dup("bnm");
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IODoubleString", din));
            const DevVarDoubleStringArray *received;
            dout >> received;

            TS_ASSERT_EQUALS(received->dvalue[0], (1.11 * 2));
            TS_ASSERT_EQUALS(received->dvalue[1], (22.2 * 2));
            TS_ASSERT(not(strcmp(received->svalue[0], "iop")));
            TS_ASSERT(not(strcmp(received->svalue[1], "jkl")));
            TS_ASSERT(not(strcmp(received->svalue[2], "bnm")));
        }


        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarDoubleStringArray in;
            in.dvalue.length(2);
            in.dvalue[0] = 1.11;
            in.dvalue[1] = 22.2;
            in.svalue.length(3);
            in.svalue[0] = Tango::string_dup("iop");
            in.svalue[1] = Tango::string_dup("jkl");
            in.svalue[2] = Tango::string_dup("bnm");
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IODoubleString", din));
            const DevVarDoubleStringArray *received;
            dout >> received;

            TS_ASSERT_EQUALS(received->dvalue[0], (in.dvalue[0] * 2));
            TS_ASSERT_EQUALS(received->dvalue[1], (in.dvalue[1] * 2));
            TS_ASSERT(not(strcmp(received->svalue[0], in.svalue[0])));
            TS_ASSERT(not(strcmp(received->svalue[1], in.svalue[1])));
            TS_ASSERT(not(strcmp(received->svalue[2], in.svalue[2])));
        }
    }

    void test_DevEncoded(void)
    {

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevEncoded in;
            in.encoded_data.length(2);
            in.encoded_data[0] = 11;
            in.encoded_data[1] = 22;
            in.encoded_format = Tango::string_dup("Sent");
            din << in;
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOEncoded", din));
            const DevEncoded *received;
            dout >> received;
            int data_type = dout.get_type();

            TS_ASSERT_EQUALS(received->encoded_data.length(), 2);
            TS_ASSERT_EQUALS(received->encoded_data[0], (11 * 2));
            TS_ASSERT_EQUALS(received->encoded_data[1], (22 * 2));
            TS_ASSERT(not(strcmp(received->encoded_format, "Returned string")));
            TS_ASSERT_EQUALS(data_type, Tango::DEV_ENCODED);
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            string in_str("Sent");
            vector<unsigned char> in_data;

            in_data.push_back((unsigned char) 15);
            in_data.push_back((unsigned char) 25);

            din.insert(in_str, in_data);
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOEncoded", din));
            DevEncoded received;
            dout >> received;
            int data_type = dout.get_type();

            TS_ASSERT_EQUALS(received.encoded_data.length(), 2);
            TS_ASSERT_EQUALS(received.encoded_data[0], (15 * 2));
            TS_ASSERT_EQUALS(received.encoded_data[1], (25 * 2));
            TS_ASSERT(not(strcmp(received.encoded_format, "Returned string")));
            TS_ASSERT_EQUALS(data_type, Tango::DEV_ENCODED);
        }

        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevVarCharArray in_data;

            in_data.length(4);
            in_data[0] = ((unsigned char) 15);
            in_data[1] = ((unsigned char) 25);
            in_data[2] = ((unsigned char) 35);
            in_data[3] = ((unsigned char) 45);

            din.insert("Sent", &in_data);
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOEncoded", din));
            DevEncoded received;
            dout >> received;
            int data_type = dout.get_type();

            TS_ASSERT_EQUALS(received.encoded_data.length(), 4);
            TS_ASSERT_EQUALS(received.encoded_data[0], (15 * 2));
            TS_ASSERT_EQUALS(received.encoded_data[1], (25 * 2));
            TS_ASSERT_EQUALS(received.encoded_data[2], (35 * 2));
            TS_ASSERT_EQUALS(received.encoded_data[3], (45 * 2));
            TS_ASSERT(not(strcmp(received.encoded_format, "Returned string")));
            TS_ASSERT_EQUALS(data_type, Tango::DEV_ENCODED);
        }
    }

    void test_DevicePipeBlob(void)
    {
        for (size_t i = 0; i < loop; i++)
        {
            DeviceData din, dout;
            DevicePipeBlob in{"TestPipeCmd"};
            vector<string> names{"level0"};
            in.set_data_elt_names(names);
            DevLong data = 123;
            in << data;
            din.insert(in);
            TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("IOPipeBlob", din));
            DevicePipeBlob received{};
            dout.extract(&received);

            TS_ASSERT_EQUALS(received.get_data_elt_nb(), 1);
            TS_ASSERT_EQUALS(received.get_data_elt_name(0), "level0");
            int received_data;
            received >> received_data;
            TS_ASSERT_EQUALS(received_data, 123);
            int data_type = dout.get_type();
            TS_ASSERT_EQUALS(data_type, Tango::DEV_PIPE_BLOB);
        }
    }
};
#undef cout
#endif // CmdTypesTestSuite_h


