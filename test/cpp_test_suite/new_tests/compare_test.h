#ifndef CompareTest_h
#define CompareTest_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>
#include <vector>

namespace CmpTst
{

	//=============================================================================
	//
	//			The CompareTest class
	//
	// description :	Class to run tests based on device output comparison. The
	//			device output is redirected to a file which is then compared with
	//			a reference output.
	//
	//=============================================================================

	class CompareTest
	{
	public:
		//+-------------------------------------------------------------------------
		//
		// method :			out_set_event_properties
		//
		// description :	Scans the output file seeking log4j:event tags and replaces
		//			values of the indicated properties in the found tags. E.g.:
		//			Original log4j:event tag:
		//			<log4j:event logger="DSERVER" timestamp="10" level="DEBUG" thread="1">
		//			After out_set_event_properties("outfile", "thread", "20") call:
		//			<log4j:event logger="DSERVER" timestamp="10" level="DEBUG" thread="20">
		// argument : in :	- file : output file to be modified
		//					- prop : property to be changed the value of
		//					- value : new value
		//
		//--------------------------------------------------------------------------
		static void out_set_event_properties(std::string file, std::string prop, std::string value);

		//+-------------------------------------------------------------------------
		//
		// method :			out_set_event_properties
		//
		// description :	Scans the output file seeking log4j:event tags and replaces
		//			values of the properties indicated by pop_val_map in the tags found.
		//			E.g.:
		//			Original log4j:event tag:
		//			<log4j:event logger="DSERVER" timestamp="10" level="DEBUG" thread="1">
		//			Map declaration:
		//				map<std::string,std::string> pop_val_map;
		//				prop_val_map["timestamp"] = "5";
		//				prop_val_map["thread"] = "20";
		//			The tag after out_set_event_properties("outfile", pop_val_map) call:
		//			<log4j:event logger="DSERVER" timestamp="5" level="DEBUG" thread="20">
		// argument : in :	- file : output file to be modified
		//					- prop_val_map : 	keys - properties to be modified,
		//										values - new values of the properties
		//
		//--------------------------------------------------------------------------
		static void out_set_event_properties(std::string file, std::map<std::string,std::string> prop_val_map);

		//+-------------------------------------------------------------------------
		//
		// method :			out_set_replace_numbers
		//
		// description :	Scans the output file seeking the occurrences of the
		//			prefix and replaces the following number with the one declared.
		//			The prefix can be any string directly preceding the number
		//			to be replaced.
		//			E.g.:
		//			Original tag in the output file:
		//			<![CDATA[In rel_monitor() DSERVER, ctr = 123, thread = 2]]>
		//			The tag after out_set_replace_numbers("outfile", "ctr = ", "4") call:
		//			<![CDATA[In rel_monitor() DSERVER, ctr = 4, thread = 2]]>
		// argument : in :	- file : 	output file to be modified
		//					- prefix : 	string directly preceding the number
		//								to be replaced
		//					- num :		new number
		//
		//--------------------------------------------------------------------------
		static void out_set_replace_numbers(std::string file, std::string prefix, std::string num);

		//+-------------------------------------------------------------------------
		//
		// method :			out_set_replace_numbers
		//
		// description :	Scans the output file seeking the occurrences of the
		//			prefixes indicated by the prefix_num_map and replaces the
		//			following numbers with the ones declared in the map.
		//			The prefix can be any string directly preceding the number
		//			to be replaced.
		//			E.g.:
		//			Original tag in the output file:
		//			<![CDATA[In rel_monitor() DSERVER, ctr = 123, thread = 2]]>
		//			Map declaration:
		//				map<std::string,std::string> prefix_num_map;
		//				prefix_num_map["ctr = "] = "4";
		//				prefix_num_map["thread = "] = "567";
		//			The tag after out_set_replace_numbers("outfile", prefix_num_map) call:
		//			<![CDATA[In rel_monitor() DSERVER, ctr = 4, thread = 567]]>
		// argument : in :	- file : 	output file to be modified
		//					- prefix_num_map : 	keys - strings preceding the numbers
		//											to be modified,
		//										values - new numbers
		//
		//--------------------------------------------------------------------------
		static void out_set_replace_numbers(std::string file, std::map<std::string,std::string> prefix_num_map);

		//+-------------------------------------------------------------------------
		//
		// method :			ref_replace_keywords
		//
		// description :	Scans the output file seeking the occurrences of the
		//			keyword and replaces them with the declared value.
		//			E.g.:
		//			Original tag in the output file:
		//			<log4j:event logger="DSERVER" timestamp="10" level="DEBUG" thread="1">
		//			The tag after ref_replace_keywords("outfile", "DSERVER", "dserver/devTest/test") call:
		//			<log4j:event logger="dserver/devTest/test" timestamp="10" level="DEBUG" thread="1">
		// argument : in :	- file : 	output file to be modified
		//					- key : 	keyword to be replaced
		//					- value :	new string
		//
		//--------------------------------------------------------------------------
		static void ref_replace_keywords(std::string file, std::string key, std::string value);

		//+-------------------------------------------------------------------------
		//
		// method :			ref_replace_keywords
		//
		// description :	Scans the output file seeking the occurrences of the
		//			keywords indicated by the key_val_map and replaces them with
		//			the declared values.
		//			E.g.:
		//			Original tag in the output file:
		//			<log4j:event logger="DSERVER" timestamp="10" level="DEBUG" thread="1">
		//			Map declaration:
		//				map<std::string,std::string> key_val_map;
		//				key_val_map["DSERVER"] = "dserver/devTest/test";
		//				key_val_map["DEBUG"] = "INFO";
		//			The tag after ref_replace_keywords("outfile", key_val_map) call:
		//			<log4j:event logger="dserver/devTest/test" timestamp="10" level="INFO" thread="1">
		// argument : in :	- file : 	output file to be modified
		//					- key_val_map : 	keys - keywords to be replaced,
		//										values - new replacement strings
		//
		//--------------------------------------------------------------------------
		static void ref_replace_keywords(std::string file, std::map<std::string,std::string> key_val_map);

		//+-------------------------------------------------------------------------
		//
		// method :			compare
		//
		// description :	Compares the output file with the reference file line
		//			by line. Throws an exception at the first occurrence of any
		//			discrepancy. No exception thrown means the files are identical.
		//			The exception indicates the number of the inconsistent line and
		//			displays it as well as the corresponding line from the reference
		//			file.
		// argument : in :	- ref : 	reference file to be compared with
		//					- out :		output file to be compared with the
		//								reference file
		//
		//--------------------------------------------------------------------------
		static void compare(std::string ref, std::string out);

		//+-------------------------------------------------------------------------
		//
		// method :			clean_on_startup
		//
		// description :	Removes the temporary files created while running
		//			the CompareTest which were left undeleted on sudden termination
		//			of a test suite.
		// argument : in :	- ref : 	reference file
		//					- out :		output file
		//
		//--------------------------------------------------------------------------
		static void clean_on_startup(std::string ref, std::string out);

		//+-------------------------------------------------------------------------
		//
		// method :			clean_up
		//
		// description :	Removes the output file and all the temporary files
		//			created while running the CompareTest like ref_tmp, or out_tmp.
		// argument : in :	- ref : 	reference file
		//					- out :		output file
		//
		//--------------------------------------------------------------------------
		static void clean_up(std::string ref, std::string out);

		//+-------------------------------------------------------------------------
		//
		// method :			leave_output
		//
		// description :	Removes the temporary files created while running
		//			the CompareTest (ref_tmp) and leaves the output file.
		// argument : in :	- ref : 	reference file
		//
		//--------------------------------------------------------------------------
		static void leave_output(std::string ref);

		//+-------------------------------------------------------------------------
		//
		// method :			print_output
		//
		// description :	Displays the output file.
		// argument : in :	- out :		output file to be displayed
		//
		//--------------------------------------------------------------------------
		static void print_output(std::string out);

		//+-------------------------------------------------------------------------
		//
		// method :			print_output
		//
		// description :	Displays the output file and shows the line numbers.
		// argument : in :	- out :					output file to be displayed
		//					- show_line_numbers :	if true displays line numbers
		//
		//--------------------------------------------------------------------------
		static void print_output(std::string out, bool show_line_numbers);

		static void out_remove_entries(std::string file, std::vector<std::string> &);
	};

	//=============================================================================
	//
	//			The CompareTestException exception
	//
	// description :	Exception thrown when CompareTest cannot open requested
	//			files or when comparison proves discrepancy between files.
	//
	//=============================================================================

	class CompareTestException : public std::runtime_error
	{
	public:
		CompareTestException(std::string err_str) : std::runtime_error(err_str) {}
//		~CompareTestException() throw() {}
	};
} // namespace CmpTst
#endif // CompareTest_h
