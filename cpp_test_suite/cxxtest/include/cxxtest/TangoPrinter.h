#ifndef __cxxtest__TangoPrinter_h__
#define __cxxtest__TangoPrinter_h__

//
// The TangoPrinter is a TestListener tailored to Tango testing needs.
// The TangoPrinter uses std::cout
//

#include <cxxtest/Flags.h>

#ifndef _CXXTEST_HAVE_STD
#   define _CXXTEST_HAVE_STD
#endif // _CXXTEST_HAVE_STD

#include <cxxtest/ErrorFormatter.h>
#include <cxxtest/StdValueTraits.h>

#ifdef _CXXTEST_OLD_STD
#   include <iostream.h>
#else // !_CXXTEST_OLD_STD
#   include <iostream>
#endif // _CXXTEST_OLD_STD

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <map>

// Tango exceptions handling
//#undef _TS_CATCH_ABORT
//#define _TS_CATCH_ABORT(b) _TS_CATCH_TYPE( (const Tango::DevFailed &e), {cout << "\n\nException : \n" << endl; Tango::Except::print_exception(e); throw;} ) _TS_CATCH_TYPE( (const CxxTest::AbortTest &), b )
#undef _TS_LAST_CATCH
#define _TS_LAST_CATCH(b) _TS_CATCH_TYPE( (const Tango::DevFailed &e), {cout << "\n\nException : \n" << endl; Tango::Except::print_exception(e); b} ) _TS_CATCH_TYPE( (...), b )

// This is a trick to deal with compiler warnings while catching DevFailed multiple times
// TODO: Remove this code when issue #53 (https://github.com/CxxTest/cxxtest/issues/53) is fixed
#undef ___TS_ASSERT_THROWS_ASSERT
#define ___TS_ASSERT_THROWS_ASSERT(f,l,e,t,a,m) { \
		bool _ts_threw_expected = false, _ts_threw_else = false; \
		_TS_TRY { _TS_TRY { e; } _TS_CATCH_TYPE( (t), { a; _ts_threw_expected = true; } ) } \
		_TS_CATCH_ABORT( { throw; } ) \
		_TS_LAST_CATCH( { _ts_threw_else = true; } ) \
		if ( !_ts_threw_expected ) { CxxTest::doFailAssertThrows( (f), (l), #e, #t, _ts_threw_else, (m) ); } }

namespace CxxTest
{
	using namespace std;

    class TangoPrinter : public ErrorFormatter
    {
    private:
    	/*
    	 * command line parameters counter
    	 */
        static int argc;
        /*
         * command line parameters array
         */
        static char **argv;
        /*
         * user arguments counter
         */
        static int uargc;
        /*
         * vector of user arguments detected automatically from the
         * command line arguments and interpreted as those which are not declared in
         * the 'params' map
         * (example: ./run_tests user_param_1 user_param_2 --loop=10 ==> uargv[0] == "user_param_1"; uargv[1] == "user_param_2")
         */
        static vector<string> uargv;
        /*
         * parameter arguments counter
         */
        static int pargc;
        /*
         * map of parameter arguments and their values obtained automatically
         * from the command line arguments as those declared in the 'params' map
         * (example: ./run_tests user_param --loop=10 ==> pargv["loop"] == "10")
         */
        static map<string,string> pargv;
        /*
         * number of times each test with '__loop' suffix is to be executed
         */
        static int loop;
        /*
         * number of times each suite with '__loop' suffix is to be executed
         */
        static int suite_loop;
        /*
         * number of times current suite with '__loop' suffix was executed
         */
        static int suite_counter;
        /*
         * map of predefined parameter names and their declarations (the way of usage in the command line)
         * (example: params["loop"] = "--loop=" ==> ./run_tests user_param --loop=10)
         */
        static map<string,vector<string>> params;
        /*
         * executable name
         */
        static string executable_name;
        /*
         * list of expected user arguments; printed out if provided arguments do not match the expected in quantity
         */
        static vector<string> uargs_validate;
        /*
         * list of expected mandatory parameters; printed out if provided arguments do not match the expected in quantity
         */
        static set<string> params_validate;
        /*
         * list of expected optional parameters; printed out if provided arguments do not match the expected in quantity
         */
        static set<string> params_opt_validate;
        /*
         * flag indicating if provided arguments are valid (in terms of quantity)
         */
        static bool args_valid;
        /*
         * set containing registered names of the methods which are to be called to restore original device settings
         * in case a test case fails before restoring these settings by itself
         */
        static set<string> restore_points;

    public:
        TangoPrinter( CXXTEST_STD(ostream) &o = CXXTEST_STD(cout), const char *preLine = ":", const char *postLine = "" ) :
            ErrorFormatter( new Adapter(o), preLine, postLine ) {}
        virtual ~TangoPrinter() { delete outputStream(); }

        /*
         * Prints out formatted name of each test suite.
         * Use the following test suite naming convention:
         * 		MySuiteNameTestSuite
         * The suffix TestSuite is removed, prefix Testing is prepended
         * and spaces are added before each capital letter.
         * As the effect we get:
         * 		Testing My Suite Name :
         */
        void enterSuite( const SuiteDescription & )
		{
        	params_opt_validate.clear();
        	params_validate.clear();
        	uargs_validate.clear();
        	restore_points.clear();
        	args_valid = true;
        	string suite_name = tracker().suite().suiteName();
        	size_t suffix_pos = suite_name.rfind("TestSuite");
        	size_t suite_name_len = suite_name.length();
			string loop_str = "__loop";
			size_t loop_str_len = loop_str.length();
			bool is_suite_loop = suite_name_len >= loop_str_len && suite_name.substr(suite_name_len - loop_str_len, loop_str_len).compare(loop_str) == 0;

        	if(suffix_pos != string::npos)
        	{
//        		suite_name.erase(suffix_pos,9);
        		suite_name.erase(suffix_pos);
        	}
        	for(size_t i = 1; i < suite_name.size(); i++)
        	{
        		if(isupper(suite_name[i]) && islower(suite_name[i-1]))
				{
        			suite_name.insert(i," ");
				}
        	}
        	if(!is_suite_loop || suite_loop < 2)
        		cout << "\nTesting " << suite_name << " :\n\n";
        	else
        		cout << "\nTesting " << suite_name << ", " << suite_loop << " iterations :\n\n";
		}

        /*
         * Checks if a test failed and exits.
         * If not, checks if the "loop" parameter has been defined
         * and runs all tests which names end with "__loop"
         * the number of times indicated by the parameter.
         */
        void leaveTest( const TestDescription &test )
        {
        	static int counter = loop - 1;
        	if ( tracker().testFailed() )
        	{
        		const SuiteDescription &suite_tmp = tracker().suite();
        		(const_cast<SuiteDescription &>(suite_tmp)).tearDown();
        		cout <<  "TEST FAILED in:\n" << loop - counter << " iteration of test case\n" << suite_counter << " iteration of test suite\n\n" ;
        		exit(-1);
            }
        	else
        	{
        		string test_name = test.testName();
        		size_t test_name_len = test_name.length();
        		string loop_str = "__loop";
        		size_t loop_str_len = loop_str.length();
        		bool is_loop = false;
        		if(test_name_len >= loop_str_len)
        			is_loop = test_name.substr(test_name_len - loop_str_len, loop_str_len).compare(loop_str) == 0;
        		if(counter > 0 && is_loop) {
        			counter--;
					(const_cast<TestDescription &>(test)).setUp();
					(const_cast<TestDescription &>(test)).run();
					(const_cast<TestDescription &>(test)).tearDown();
					leaveTest(test);
        		}
        		else
        		{
        			// removes 'test' prefix from the test name
					if(test_name.find("test_") == 0)
						test_name.erase(0,5);
					else if(test_name.find("test") == 0)
						test_name.erase(0,4);

					// removes the "__loop" suffix from the test name
					size_t loop_pos = test_name.rfind(loop_str);
					if(loop_pos != string::npos && loop_pos == test_name.length() - loop_str.length())
						test_name.erase(loop_pos,loop_str.length());

					// replaces underscores '_' with spaces ' ' in the test name
					for(size_t i = 0; i < test_name.size(); i++)
					{
						if(test_name[i] == '_')
							test_name[i] = ' ';
					}

					// prints out the the success notice only after the last iteration
					// of test case execution in the last iteration of the test suite run
					string suite_name = tracker().suite().suiteName();
					size_t suite_name_len = suite_name.length();
					string loop_str = "__loop";
					size_t loop_str_len = loop_str.length();
					bool is_suite_loop = false;
					if(suite_name_len >= loop_str_len)
						is_suite_loop = suite_name.substr(suite_name_len - loop_str_len, loop_str_len).compare(loop_str) == 0;
					if(!is_suite_loop || (is_suite_loop && suite_counter >= suite_loop))
					{
						if(!is_loop || loop < 2)
							cout << "\t" << test_name << " --> OK\n";
						else
							cout << "\t" << test_name << ", " << loop << " iterations --> OK\n";
					}

        			counter = loop - 1;
        		}
        	}
        }

        void leaveSuite( const SuiteDescription &suite )
        {
    		string suite_name = suite.suiteName();
    		size_t suite_name_len = suite_name.length();
    		string loop_str = "__loop";
    		size_t loop_str_len = loop_str.length();
    		if(suite_counter < suite_loop && suite_name_len >= loop_str_len && suite_name.substr(suite_name_len - loop_str_len, loop_str_len).compare(loop_str) == 0) {
				unsigned int num_tests = (const_cast<SuiteDescription &>(suite)).numTests();
    			while(suite_counter < suite_loop)
    			{
    	        	params_opt_validate.clear();
    	        	params_validate.clear();
    	        	uargs_validate.clear();
    	        	restore_points.clear();
    	        	args_valid = true;
    				suite_counter++;
    				TestDescription *test = (const_cast<SuiteDescription &>(suite)).firstTest();
					(const_cast<SuiteDescription &>(suite)).setUp();
					unsigned int test_counter = 0;
					while(test_counter < num_tests && test != 0)
					{
						tracker().enterTest(*test);
						test->setUp();
						test->run();
						leaveTest(*test);
						test->tearDown();
						test = test->next();
						test_counter++;
					}
					(const_cast<SuiteDescription &>(suite)).tearDown();
    			}
    			suite_counter = 1;
    		}
        	cout << "\n";
        }

        int run( int argc_tmp, char **argv_tmp )
		{
        	argc = argc_tmp;
        	argv = argv_tmp;

        	if(argc >= 1)
        		executable_name = argv[0];

        	// compares if argument list contains any parameter from predefined list
        	for(int i = 1; i < argc; i++)
        	{
        		string arg_tmp = argv[i];
        		bool is_param = false;

        		for(map<string,vector<string>>::iterator it = params.begin(); it != params.end(); ++it)
        		{
					string param_key = it->first;
        			string param_value = it->second[0];
        			string param_desc = it->second[1];

        			int param_value_length = param_value.length();
        			if(param_value.compare(arg_tmp.substr(0, param_value_length)) == 0)
        			{
        				// checks if the parameter has already been used to accept value of only the first occurrence of the parameter
        				if(pargv.size() > 0 && pargv.find(param_key) != pargv.end())
        				{
        					is_param = true;
							break;
						}
        				is_param = true;
        				pargv[param_key] = arg_tmp.substr(param_value_length, arg_tmp.length() - param_value_length);
        				pargc = pargv.size();
        				break;
        			}
        		}
        		if(!is_param)
        			uargv.push_back(argv[i]);
        	}
        	uargc = uargv.size();

        	// prints out the list of all parameters if required by user ("--?" or "--help")
        	if(TangoPrinter::is_param_set("?") || TangoPrinter::is_param_set("help"))
        	{
        		cout << "\nAll parameters:";
        		for(map<string,vector<string>>::iterator it = TangoPrinter::params.begin(); it != TangoPrinter::params.end(); ++it)
        		{
        			cout << "\n\t" << it->second[0] << " - " << it->second[1];
        		}
        		cout << "\n";
        		exit(0);
        	}

        	// atoi() converts string into integer; if not possible, returns 0
        	if(is_param_defined("loop"))
        		loop = atoi(get_param_val("loop").c_str());

        	// atoi() converts string into integer; if not possible, returns 0
        	if(is_param_defined("suiteloop"))
        		suite_loop = atoi(get_param_val("suiteloop").c_str());

			TestRunner::runAllTests( *this );
			return tracker().failedTests();
		}

        /*
         * registers a restore point to take up an action in TestSuite tearDown method
         * in case a test case does not restore the default device properties
         */
        static void restore_set(const char* name)
        {
        	restore_set(string(name));
        }

        /*
         * registers a restore point to take up an action in TestSuite tearDown method
         * in case a test case does not restore the default device properties
         */
        static void restore_set(const string &name)
        {
        	restore_points.insert(name);
        }

        /*
		 * unregisters the restore point (after the test case had successfully restored the default device properties)
		 */
        static void restore_unset(const char* name)
		{
			restore_unset(string(name));
		}

        /*
		 * unregisters the restore point (after the test case had successfully restored the default device properties)
         */
        static void restore_unset(const string &name)
        {
        	restore_points.erase(name);
        }

        /*
         * checks if the restore point has been registered
         */
		static bool is_restore_set(const char* name)
		{
		   return is_restore_set(string(name));
		}

        /*
         * checks if the restore point has been registered
         */
        static bool is_restore_set(const string &name)
        {
        	bool result = false;
        	if(!restore_points.empty())
        		if(restore_points.find(name) != restore_points.end())
        			result = true;
        	return result;
        }

        static unsigned int get_argc(void)
        {
        	return argc;
        }

        static char **get_argv(void)
        {
        	return argv;
        }

        static vector<string> &get_uargv(void)
		{
        	return uargv;
        }

        static unsigned int get_uargc(void)
		{
			return uargc;
		}

        /*
         * returns number of predefined parameters used in command line by user
         */
        static unsigned int get_pargc(void)
		{
			return pargc;
		}

        /*
         * checks if parameter of given name was used by user in command line
         */
        static bool is_param_set(const string &key)
        {
        	if(pargv.size() > 0 && pargv.find(key) != pargv.end())
				return true;
			else
				return false;
        }

        /*
         * returns value of the parameter of given name in the form of string
         */
        static string get_param_val(const string &key)
        {
        	if(pargv.size() > 0 && pargv.find(key) != pargv.end())
        		return pargv[key];
        	else
        		return "";
        }

        /*
         * checks if parameter of given name was defined
         */
        static bool is_param_defined(const string &key)
		{
			if(params.size() > 0 && params.find(key) != params.end())
				return true;
			else
				return false;
		}

        /*
         * returns parameter definition based on it's name
         */
        static string get_param_def(const string &key)
		{
			if(params.size() > 0 && params.find(key) != params.end())
				return params[key][0];
			else
				return "";
		}

        /*
         * returns parameter description based on it's name
         */
        static string get_param_desc(const string &def)
		{
			if(params.size())// > 0 && params.find(key) != params.end())
				for(map<string,vector<string>>::iterator it = params.begin(); it != params.end(); ++it)
					if(it->second[0] == def)
						return it->second[1];
			return "";
		}

        static map<string,vector<string>> &get_params(void)
		{
			return params;
		}

        static string &get_executable_name(void)
        {
        	return executable_name;
        }

        /*
         * registers description of user argument on the list, returns its value (if set) or empty string
         */
        static string get_uarg(const string &uarg)
        {
        	string uarg_val = "";
        	uargs_validate.push_back(uarg);
        	if(get_uargc() >= uargs_validate.size())
        		uarg_val = get_uargv()[uargs_validate.size()-1];
        	else
        		args_valid = false;
        	return uarg_val;
        }

        /*
         * registers description of mandatory parameter on the list, returns its value (if set) or empty string
         */
        static string get_param(const string &param)
        {
        	string param_val = "";
        	if(is_param_defined(param))
        	{
				params_validate.insert(get_param_def(param));
				if(!is_param_set(param))
					args_valid = false;
				param_val = get_param_val(param);
        	}
        	return param_val;
        }

        /*
         * registers description of optional parameter on the list, returns its value (if set) or empty string
         */
        static string get_param_opt(const string &param)
        {
        	string param_val = "";
        	if(is_param_defined(param))
        	{
				params_opt_validate.insert(get_param_def(param));
				param_val = get_param_val(param);
        	}
        	return param_val;
        }

        /*
         * registers description of optional parameter on the list, returns true if parameter has been set
         * (useful when no value for parameter is expected e.g. "--v")
         */
        static bool is_param_opt_set(const string &param)
        {
        	bool param_opt_set = false;
        	if(is_param_defined(param))
        	{
				params_opt_validate.insert(get_param_def(param));
				if(is_param_set(param))
					param_opt_set = true;
        	}
        	return param_opt_set;
        }

        /*
         * checks if all required arguments have been properly set, if not prints out usage hint and terminates
         */
        static void validate_args()
        {
        	if(!args_valid)
        	{
    			cout << "Usage: " << get_executable_name();

    			for(size_t i = 0; i < uargs_validate.size(); i++)
    				cout << " " << uargs_validate[i];

    			if(params_validate.size() != 0 || params_opt_validate.size() != 0)
    			{
        			for(set<string>::iterator it = params_validate.begin(); it != params_validate.end(); ++it)
        				cout << " " << *it;

        			for(set<string>::iterator it = params_opt_validate.begin(); it != params_opt_validate.end(); ++it)
        				cout << " [" << *it << "]";

    				cout << "\nParameters explanation:";
    				if(params_validate.size() != 0)
    				{
    					cout << "\nMandatory:";
            			for(set<string>::iterator it = params_validate.begin(); it != params_validate.end(); ++it)
            				cout << "\n\t" << *it << " - " << get_param_desc(*it);
    				}

    				if(params_opt_validate.size() != 0)
    				{
    					cout << "\nOptional:";
            			for(set<string>::iterator it = params_opt_validate.begin(); it != params_opt_validate.end(); ++it)
            				cout << "\n\t" << *it << " - " << get_param_desc(*it);
    				}
    			}

    			cout  << "\n";
    			exit(-1);
        	}
        }


        /*
         * helper function to create a vector containing a parameter description
         */
        static vector<string> param_desc(string param, string desc)
    	{
        	vector<string> param_desc_tmp;
        	param_desc_tmp.push_back(param);
        	param_desc_tmp.push_back(desc);
        	return param_desc_tmp;
    	}

        /*
         * declare predefined parameters here
         */
        static map<string,vector<string>> create_params()
		{
        	map<string,vector<string> > params_tmp;
        	params_tmp["?"] = param_desc("--?", "help, lists all possible parameters");
        	params_tmp["help"] = param_desc("--help", "help, lists all possible parameters");
        	params_tmp["verbose"] = param_desc("--v", "verbose mode");
        	params_tmp["loop"] = param_desc("--loop=","execute test cases marked with '__loop' suffix the indicated number of times");
        	params_tmp["suiteloop"] = param_desc("--suiteloop=", "execute test suites marked with '__loop' suffix the indicated number of times"); // executes suite in a loop
        	params_tmp["fulldsname"] = param_desc("--fulldsname=", "full device server name, e.g. devTest/myserver");
        	params_tmp["clienthost"] = param_desc("--clienthost=", "client host's fully qualified domain name, e.g. mypc.myinstitute.com (small caps)");
        	params_tmp["serverhost"] = param_desc("--serverhost=", "fully qualified domain name of the host on which the server is running, e.g. myserver.myinstitute.com (small caps)");
        	params_tmp["serverversion"] = param_desc("--serverversion=", "IDL version, e.g. 4");
        	params_tmp["dbserver"] = param_desc("--dbserver=", "database server name, e.g. sys/database/2");
        	params_tmp["loglevel"] = param_desc("--loglevel=", "default device logging level, e.g. 0"); // default device logging level, e.g. 0
        	params_tmp["dsloglevel"] = param_desc("--dsloglevel=", "default device server logging level, e.g. 3"); // default device server logging level, e.g. 3
        	params_tmp["devtype"] = param_desc("--devtype=", "device type, e.g. TestDevice");
        	params_tmp["docurl"] = param_desc("--docurl=", "current documentation URL, e.g. http://www.tango-controls.org");
        	params_tmp["outpath"] = param_desc("--outpath=", "device server logging target directory, e.g. /tmp/"); // device server logging target directory, e.g. /tmp/
        	params_tmp["refpath"] = param_desc("--refpath=", "directory where the 'compare test' reference files (*.out) are stored"); // directory where the compare test reference files (*.out) are stored
        	params_tmp["devicealias"] = param_desc("--devicealias=", "device1 alias"); // device1 alias
        	params_tmp["attributealias"] = param_desc("--attributealias=", "Short_attr alias"); // Short_attr alias
        	return params_tmp;
        }

    private:
        class Adapter : public OutputStream
        {
            CXXTEST_STD(ostream) &_o;
        public:
            Adapter( CXXTEST_STD(ostream) &o ) : _o(o) {}
            void flush() { _o.flush(); }
            OutputStream &operator<<( const char *s ) { _o << s; return *this; }
            OutputStream &operator<<( Manipulator m ) { return OutputStream::operator<<( m ); }
            OutputStream &operator<<( unsigned i )
            {
                char s[1 + 3 * sizeof(unsigned)];
                numberToString( i, s );
                _o << s;
                return *this;
            }
        };
    };

    int TangoPrinter::argc = 0;
    char **TangoPrinter::argv = 0;
    int TangoPrinter::uargc = 0;
	vector<string> TangoPrinter::uargv;
    int TangoPrinter::pargc = 0;
	map<string,string> TangoPrinter::pargv;
    int TangoPrinter::loop = 0;
    map<string,vector<string> > TangoPrinter::params = TangoPrinter::create_params();
    string TangoPrinter::executable_name = "";
    int TangoPrinter::suite_loop = 0;
    int TangoPrinter::suite_counter = 1;

    vector<string> TangoPrinter::uargs_validate;
    set<string> TangoPrinter::params_validate;
    set<string> TangoPrinter::params_opt_validate;
    bool TangoPrinter::args_valid = true;

    set<string> TangoPrinter::restore_points = set<string>();
}

#endif // __cxxtest__TangoPrinter_h__
