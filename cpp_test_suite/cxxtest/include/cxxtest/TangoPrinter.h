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

namespace CxxTest 
{
	using namespace std;

    class TangoPrinter : public ErrorFormatter
    {
    private:
    	/*
    	 * command line parametres counter
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
        static int loop;
        /*
         * map of predefined parameter names and their declarations (the way of usage in the command line)
         * (example: params["loop"] = "--loop=" ==> ./run_tests user_param --loop=10)
         */
        static map<string,string> params;
        /*
         * executable name
         */
        static string executable_name;

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
        void enterSuite( const SuiteDescription &desc )
		{
        	string suite_name = tracker().suite().suiteName();
        	size_t suffix_pos = suite_name.rfind("TestSuite");
        	if(suffix_pos != string::npos)
        	{
        		suite_name.erase(suffix_pos,9);
        	}
        	for(size_t i = 1; i < suite_name.size(); i++)
        	{
        		if(isupper(suite_name[i]) && islower(suite_name[i-1]))
				{
        			suite_name.insert(i," ");
				}
        	}
        	cout << "\nTesting " << suite_name << " :\n\n";
		}

        /*
         * Checks if a test failed and exits.
         * If not, checks if the "loop" parameter has been defined
         * and runs all tests which names end with "__loop"
         * the number of times indicated by the parameter.
         */
        void leaveTest( const TestDescription &desc )
        {
        	static int counter = loop - 1;
        	if ( tracker().testFailed() )
        	{
        		const SuiteDescription &suite_tmp = tracker().suite();
        		(const_cast<SuiteDescription &>(suite_tmp)).tearDown();
        		cout <<  "\nTEST FAILED in " << loop - counter << ". iteration\n" ;
        		exit(-1);
            }
        	else
        	{
        		string test_name = tracker().test().testName();
        		int test_name_len = test_name.length();
        		string loop_str = "__loop";
        		int loop_str_len = loop_str.length();
        		if(counter > 0 && test_name_len >= loop_str_len && test_name.substr(test_name_len - loop_str_len, loop_str_len).compare(loop_str) == 0) {
        			counter--;
        			const TestDescription &test = tracker().test();
					(const_cast<TestDescription &>(test)).setUp();
					(const_cast<TestDescription &>(test)).run();
					(const_cast<TestDescription &>(test)).tearDown();
					leaveTest(desc);
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
					if(loop_pos == test_name.length() - loop_str.length())
						test_name.erase(loop_pos,loop_str.length());

					// replaces undersocres '_' with spaces ' ' in the test name
					for(size_t i = 0; i < test_name.size(); i++)
					{
						if(test_name[i] == '_')
							test_name[i] = ' ';
					}

        			cout << "\t" << test_name << " --> OK" << "\n" ;
        			counter = loop - 1;
        		}
        	}
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

        		for(map<string,string>::iterator it = params.begin(); it != params.end(); ++it)
        		{
					string param_key = it->first;
        			string param_value = it->second;

        			int param_value_length = param_value.length();
        			if(param_value.compare(arg_tmp.substr(0, param_value_length)) == 0)
        			{
        				// checks if the parameter has already been used to accept value of only the first occurance of the parameter
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
        		cout << "\navailable parameters:";
        		for(map<string,string>::iterator it = TangoPrinter::params.begin(); it != TangoPrinter::params.end(); ++it)
        		{
        			cout << " " << it->second;
        		}
        		cout << "\n";
        		exit(0);
        	}

        	// atoi() converts string into integer; if not possible, returns 0
        	if(is_param_defined("loop"))
        		loop = atoi(get_param_val("loop").c_str());

			TestRunner::runAllTests( *this );
			return tracker().failedTests();
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
        static bool is_param_set(const string key)
        {
        	if(pargv.size() > 0 && pargv.find(key) != pargv.end())
				return true;
			else
				return false;
        }

        /*
         * returns value of the parameter of given name in the form of string
         */
        static string get_param_val(const string key)
        {
        	if(pargv.size() > 0 && pargv.find(key) != pargv.end())
        		return pargv[key];
        	else
        		return "";
        }

        /*
         * checks if parameter of given name was defined
         */
        static bool is_param_defined(const string key)
		{
			if(params.size() > 0 && params.find(key) != params.end())
				return true;
			else
				return false;
		}

        /*
         * returns parameter definition based on it's name
         */
        static string get_param_def(const string key)
		{
			if(params.size() > 0 && params.find(key) != params.end())
				return params[key];
			else
				return "";
		}

        static map<string,string> &get_params(void)
		{
			return params;
		}

        static string &get_executable_name(void)
        {
        	return executable_name;
        }

        /*
         * declare predefined parameters here
         */
        static map<string,string> create_params()
		{
        	map<string,string> params_tmp;
        	params_tmp["loop"] = "--loop=";
        	params_tmp["verbose"] = "--v";
        	params_tmp["fulldsname"] = "--fulldsname=";
        	params_tmp["serverhost"] = "--serverhost=";
        	params_tmp["serverversion"] = "--serverversion=";
        	params_tmp["docurl"] = "--docurl=";
        	params_tmp["devtype"] = "--devtype=";
        	params_tmp["?"] = "--?";
        	params_tmp["help"] = "--help";
        	params_tmp["dbserver"] = "--dbserver=";
        	params_tmp["outpath"] = "--outpath="; // device server logging target directory, e.g. /tmp/
        	params_tmp["refpath"] = "--refpath="; // directory where the compare test reference files (*.out) are stored
        	params_tmp["loglevel"] = "--loglevel="; // default device logging level, e.g. 0
        	params_tmp["dsloglevel"] = "--dsloglevel="; // default device server logging level, e.g. 3
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
    map<string,string> TangoPrinter::params = TangoPrinter::create_params();
    string TangoPrinter::executable_name = "";
}

#endif // __cxxtest__TangoPrinter_h__
