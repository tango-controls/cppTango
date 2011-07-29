#ifndef CompareTest_h
#define CompareTest_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>

namespace CmpTst
{
	class CompareTest
	{
	public:
		static void out_set_event_properties(std::string file, std::string value, std::string prop);
		static void out_set_event_properties(std::string file, std::map<std::string,std::string> prop_val_map);
		static void out_set_replace_numbers(std::string file, std::string prefix, std::string num);
		static void out_set_replace_numbers(std::string file, std::map<std::string,std::string> prefix_num_map);
		static void ref_replace_keywords(std::string file, std::string key, std::string prop);
		static void ref_replace_keywords(std::string file, std::map<std::string,std::string> key_val_map);
		static void compare(std::string ref, std::string out);
		static void clean_up(std::string ref, std::string out);
		static void print_output(std::string out);
		static void print_output(std::string out, bool show_line_numbers);
	};

	class CompareTestException : public std::runtime_error
	{
	public:
		CompareTestException(std::string err_str) : std::runtime_error(err_str) {}
//		~CompareTestException() throw() {}
	};
} // namespace CmpTst
#endif // CompareTest_h
