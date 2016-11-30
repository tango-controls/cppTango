Note for developers: All considerations mentioned below have been fixed by redefining _TS_CATCH_ABORT() macro in TangoPrinter.h.
#In order to print out Tango::DevFailed exception message when TS_THROWS_NOTHING() assertion fails, TestSuite.h file has been modified.
#	TestSuite.h:15	- tango.h has been included
#	TestSuite.h:196	- Tango::DevFailed exception is caught at first place
#	TestSuite.h:462	- calling macro capable of catching Tango::DevFailed exception
#This is a modification of the core components of CxxTest and requires an alternative solution!