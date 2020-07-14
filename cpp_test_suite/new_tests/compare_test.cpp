#include "compare_test.h"
#include <stdio.h>
#include <algorithm>

using namespace std;
using namespace CmpTst;

#define TMP_SUFFIX ".tmp"

namespace
{

const std::string LOG_PREFIX = "<log4j:message><![CDATA[";
const std::string LOG_SUFFIX = "]]></log4j:message>";

void strip_cr_lf(std::string& line)
{
    while (line.size() > 0 && (line.back() == '\r' || line.back() == '\n'))
    {
        line.pop_back();
    }
}

bool is_log_message(const std::string& line)
{
    return line.compare(0, LOG_PREFIX.size(), LOG_PREFIX) == 0
        && line.compare(line.size() - LOG_SUFFIX.size(), LOG_SUFFIX.size(), LOG_SUFFIX) == 0;
}

auto strip_log_tags(const std::string& line)
{
    return line.substr(LOG_PREFIX.size(), line.size() - LOG_PREFIX.size() - LOG_SUFFIX.size());
}

bool is_matching_log_message(const std::string& ref_line, const std::string& out_line)
{
    // For log messages, we only check that emitted message contains expected
    // substring. There may be extra information included (like line numbers).

    return is_log_message(ref_line) && is_log_message(out_line)
        && out_line.find(strip_log_tags(ref_line)) != std::string::npos;
}

bool is_line_equal(const std::string& ref_line, const std::string& out_line)
{
    return ref_line.compare(out_line) == 0;
}

bool compare_lines(const std::string& ref_line, const std::string& out_line)
{
    return is_line_equal(ref_line, out_line) || is_matching_log_message(ref_line, out_line);
}

} // namespace

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
void CmpTst::CompareTest::out_set_event_properties(string file, string prop, string val)
{
	map<string,string> prop_val_map;
	prop_val_map[prop] = val;
	out_set_event_properties(file, prop_val_map);
}

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
void CmpTst::CompareTest::out_set_event_properties(string file, map<string,string> prop_val_map)
{
	if(prop_val_map.size() > 0)
	{
		ifstream infile;
		ofstream outfile;

		infile.open(file.c_str()); // file to be modified
		if(!infile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot open file: " + file);

		string outfile_name = file + TMP_SUFFIX; // temporary file
		outfile.open(string(outfile_name).c_str());
		if(!outfile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot open file for writing: " + outfile_name);

		while(infile)
		{
			string line;
			getline(infile, line);

			// modify only properties of the 'event' tags
			if(line.find("<log4j:event ") != string::npos)
			{
				// for each line checks presence of each property
				for(map<string,string>::iterator it = prop_val_map.begin(); it != prop_val_map.end(); ++it)
				{
					string property_str = (*it).first + "=\"";
					// replaces property with value each time the keyword is found in the line
					size_t begin = line.find(property_str);
					while(begin != string::npos)
					{
						begin += property_str.length();
						size_t end = line.find("\"", begin); // looking for the property value closing double quotation mark
						line.replace(begin, end - begin, (*it).second); // replacing with new value
						begin = line.find(property_str, begin + (*it).second.size());
					}
				}
			}

			outfile << line; // writing line to the tmp file
			if(!infile.eof())
				outfile << endl; // no endl after the last line in the file
		}

		infile.close();
		outfile.close();
		if(infile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot close file: " + file);
		if(outfile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot close file: " + outfile_name);

		// remove file to swap its content with the tmp file
		if(remove(file.c_str()) != 0)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot remove file: " + file);

		// rename the tmp file to the name of its original
		if(rename(string(outfile_name).c_str(), file.c_str()) != 0)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot rename file: " + outfile_name);
	}
}

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
void CmpTst::CompareTest::out_set_replace_numbers(string file, string prefix, string num)
{
	map<string,string> prefix_num_map;
	prefix_num_map[prefix] = num;
	out_set_replace_numbers(file, prefix_num_map);
}

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
void CmpTst::CompareTest::out_set_replace_numbers(string file, map<string,string> prefix_num_map)
{
	if(prefix_num_map.size() > 0)
	{
		ifstream infile;
		ofstream outfile;

		infile.open(file.c_str()); // file to be modified
		if(!infile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot open file: " + file);

		string outfile_name = file + TMP_SUFFIX; // temporary file
		outfile.open(string(outfile_name).c_str());
		if(!outfile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot open file for writing: " + outfile_name);

		while(infile)
		{
			string line;
			getline(infile, line);

			// for each line checks presence of each property
			for(map<string,string>::iterator it = prefix_num_map.begin(); it != prefix_num_map.end(); ++it)
			{
				string prefix = (*it).first;
				// replaces property with value each time the prefix is found in the line
				size_t begin = line.find(prefix);
				while(begin != string::npos)
				{
					begin += prefix.size();

					size_t i = begin;
					// checks if the next character is a number
					while(i < line.size() && line[i] >= '0' && line[i] <='9')
						i++;

					size_t end = i;
					line.replace(begin, end - begin, (*it).second);
					begin = line.find(prefix, begin + (*it).second.size());
				}
			}

			outfile << line; // writing line to the tmp file
			if(!infile.eof())
				outfile << endl; // no endl after the last line in the file
		}

		infile.close();
		outfile.close();
		if(infile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot close file: " + file);
		if(outfile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot close file: " + outfile_name);

		// remove file to swap its content with the tmp file
		if(remove(file.c_str()) != 0)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot remove file: " + file);

		// rename the tmp file to the name of its original
		if(rename(string(outfile_name).c_str(), file.c_str()) != 0)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_set_event_properties] Cannot rename file: " + outfile_name);
	}
}

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
void CmpTst::CompareTest::ref_replace_keywords(string file, string key, string val)
{
	map<string,string> key_val_map;
	key_val_map[key] = val;
	ref_replace_keywords(file, key_val_map);
}

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
void CmpTst::CompareTest::ref_replace_keywords(string file, map<string,string> key_val_map)
{
	if(key_val_map.size() > 0)
	{
		ifstream infile;
		ofstream outfile;

		infile.open(file.c_str());
		if(!infile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::ref_replace_keywords] Cannot open file: " + file);

		string outfile_name = file + TMP_SUFFIX; // temporary file
		outfile.open(string(outfile_name).c_str());
		if(!outfile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::ref_replace_keywords] Cannot open file for writing: " + outfile_name);

		while(infile)
		{
			string line;
			getline(infile, line);

			// for each line checks presence of each keyword
			for(map<string,string>::iterator it = key_val_map.begin(); it != key_val_map.end(); ++it)
			{
				// replaces keyword with value each time the keyword is found in the line
				size_t begin = line.find((*it).first);
				while(begin != string::npos)
				{
					line.replace(begin, (*it).first.size(), (*it).second);
					begin = line.find((*it).first, begin + (*it).second.size());
				}
			}

			outfile << line; // writing line to the tmp file
			if(!infile.eof())
				outfile << endl; // no endl after the last line in the file
		}

		infile.close();
		outfile.close();
		if(infile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::ref_replace_keywords] Cannot close file: " + file);
		if(outfile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::ref_replace_keywords] Cannot close file: " + outfile_name);
	}
}

//+-------------------------------------------------------------------------
//
// method :			out_remove_entries
//
// description :
// argument : in :	- file : output file to be modified
//					- prop_val_map : 	keys - properties to be modified,
//										values - new values of the properties
//
//--------------------------------------------------------------------------
void CmpTst::CompareTest::out_remove_entries(string file, vector<string> &v_entries)
{
	if(v_entries.empty() == false)
	{
		ifstream infile;
		ofstream outfile;

		infile.open(file.c_str()); // file to be modified
		if(!infile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_remove_entries] Cannot open file: " + file);

		string outfile_name = file + TMP_SUFFIX; // temporary file
		outfile.open(string(outfile_name).c_str());
		if(!outfile)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_remove_entries] Cannot open file for writing: " + outfile_name);

		while(infile)
		{
			string line1;
			getline(infile, line1);

			if (line1.find("<log4j:event ") == string::npos)
			{
				outfile << line1;
				continue;
			}

			string line2,line3,line4,line5;
			getline(infile,line2);
			getline(infile,line3);
			getline(infile,line4);
			getline(infile,line5);

			// for each line checks presence of each property

			bool found = false;
			for(vector<string>::iterator it = v_entries.begin(); it != v_entries.end(); ++it)
			{
				if (line2.find(*it) != string::npos)
				{
					found = true;
					break;
				}
			}

			if (found == true)
				continue;

			outfile << line1; // writing lines to the tmp file
			outfile << endl;
			outfile << line2;
			outfile << endl;
			outfile << line3;
			outfile << endl;
			outfile << line4;
			outfile << endl;
			outfile << line5;


			if(!infile.eof())
				outfile << endl; // no endl after the last line in the file
		}

		infile.close();
		outfile.close();
		if(infile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_remove_entries] Cannot close file: " + file);
		if(outfile.bad())
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_remove_entries] Cannot close file: " + outfile_name);

		// remove file to swap its content with the tmp file
		if(remove(file.c_str()) != 0)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_reemove_entries] Cannot remove file: " + file);

		// rename the tmp file to the name of its original
		if(rename(string(outfile_name).c_str(), file.c_str()) != 0)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::out_remove_entries] Cannot rename file: " + outfile_name);
	}
}

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
void CmpTst::CompareTest::compare(string ref, string out)
{
	ifstream refstream, outstream;

	string ref_tmp = ref + TMP_SUFFIX;
	refstream.open(ref_tmp.c_str());
	if(!refstream)
	{
		refstream.open(ref.c_str());
		if(!refstream)
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] Cannot open reference file: " + ref);
	}

	outstream.open(out.c_str());
	if(!outstream)
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] Cannot open output file: " + out);

	unsigned int line_number = 0;
	while(refstream && outstream)
	{
		line_number++;
		string ref_line, out_line;
		getline(refstream, ref_line);
		getline(outstream, out_line);

		strip_cr_lf(ref_line);
		strip_cr_lf(out_line);

		if (!compare_lines(ref_line, out_line))
		{
			refstream.close();
			outstream.close();
			stringstream ss;
			ss << line_number;
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] FAILED in file: " + out + ":" + ss.str() + "\nEXPECTED:\t" + ref_line + "\n     WAS:\t" + out_line);
		}
	}

	refstream.close();
	outstream.close();
	if(refstream.bad())
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] Cannot close file: " + ref);
	if(outstream.bad())
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] Cannot close file: " + out);
}

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
void CmpTst::CompareTest::clean_on_startup(string ref, string out)
{
	string ref_tmp = ref + TMP_SUFFIX;
	remove(ref_tmp.c_str());
	remove(out.c_str());
}

//+-------------------------------------------------------------------------
//
// method :			clean_up
//
// description :	Removes the output file and the temporary files
//			created while running the CompareTest (ref_tmp).
// argument : in :	- ref : 	reference file
//					- out :		output file
//
//--------------------------------------------------------------------------
void CmpTst::CompareTest::clean_up(string ref, string out)
{
	string ref_tmp = ref + TMP_SUFFIX;
	bool ref_err = false, out_err = false;

	if(remove(ref_tmp.c_str()) != 0)
		ref_err = true;

	if(remove(out.c_str()) != 0)
		out_err = true;

	if(ref_err)
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::clean_up] Cannot remove file: " + ref_tmp);

	if(out_err)
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::clean_up] Cannot remove file: " + out);
}

//+-------------------------------------------------------------------------
//
// method :			leave_output
//
// description :	Removes the temporary files created while running
//			the CompareTest (ref_tmp) and leaves the output file.
// argument : in :	- ref : 	reference file
//
//--------------------------------------------------------------------------
void CmpTst::CompareTest::leave_output(string ref)
{
	string ref_tmp = ref + TMP_SUFFIX;

	if(remove(ref_tmp.c_str()) != 0)
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::clean_up] Cannot remove file: " + ref_tmp);
}

//+-------------------------------------------------------------------------
//
// method :			print_output
//
// description :	Displays the output file.
// argument : in :	- out :		output file to be displayed
//
//--------------------------------------------------------------------------
void CmpTst::CompareTest::print_output(string out)
{
	print_output(out, false);
}

//+-------------------------------------------------------------------------
//
// method :			print_output
//
// description :	Displays the output file and shows the line numbers.
// argument : in :	- out :					output file to be displayed
//					- show_line_numbers :	if true displays line numbers
//
//--------------------------------------------------------------------------
void CmpTst::CompareTest::print_output(string out, bool show_line_numbers)
{
	ifstream outstream;

	outstream.open(out.c_str());
	if(!outstream)
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::print_output] Cannot open output file: " + out);

	cout << out << ":" << endl;
	unsigned int line_number = 0;
	while(outstream)
	{
		line_number++;
		string ref_line, out_line, ref_line_orig, out_line_orig;
		getline(outstream, out_line);
		if(show_line_numbers)
			cout << line_number << "\t" << out_line << endl;
		else
			cout << out_line << endl;
	}

	outstream.close();
	if(outstream.bad())
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::print_output] Cannot close file: " + out);
}

#undef TMP_SUFFIX
