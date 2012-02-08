------------------------------------
Cxx Test Suite for the Tango library
------------------------------------

Before running the test suite make sure that:

1. 	Tango Starter process is not running. To check if the process is running type in command line:
	ps -ef | grep -i starter
	
2.	Execute conf_devtest which configures devTest properties in the database.

WARNING:

1. 	Upon execution of conf_devtest a new pseudo device server is registered in the database, namely: DsCache/test.
	Make sure no server of such name exists, otherwise refrain from executing conf_devtest.
