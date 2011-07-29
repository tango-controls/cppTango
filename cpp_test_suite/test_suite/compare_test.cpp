#include "compare_test.h"
#include <stdio.h>

using namespace std;
using namespace CmpTst;

#define TMP_SUFFIX ".tmp"

void CmpTst::CompareTest::out_set_event_properties(string file, string prop, string val)
{
	map<string,string> prop_val_map;
	prop_val_map[prop] = val;
	out_set_event_properties(file, prop_val_map);
}

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

void CmpTst::CompareTest::out_set_replace_numbers(string file, string prefix, string num)
{
	map<string,string> prefix_num_map;
	prefix_num_map[prefix] = num;
	out_set_replace_numbers(file, prefix_num_map);
}

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

void CmpTst::CompareTest::ref_replace_keywords(string file, string key, string val)
{
	map<string,string> key_val_map;
	key_val_map[key] = val;
	ref_replace_keywords(file, key_val_map);
}

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
		string ref_line, out_line, ref_line_orig, out_line_orig;
		getline(refstream, ref_line);
		getline(outstream, out_line);

		ref_line_orig = ref_line; // stores original line for error message, see linebreak hook below
		out_line_orig = out_line;
		size_t end_line = min(ref_line.size(), out_line.size());
		out_line.erase(end_line); // trick to make it work with different linebreak encoding;
		if(ref_line.compare(out_line) != 0)
		{
			refstream.close();
			outstream.close();
			stringstream ss;
			ss << line_number;
			throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] FAILED in file: " + out + ":" + ss.str() + "\nEXPECTED:\t" + ref_line_orig + "\n     WAS:\t" + out_line_orig);
		}
	}

	refstream.close();
	outstream.close();
	if(refstream.bad())
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] Cannot close file: " + ref);
	if(outstream.bad())
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::compare] Cannot close file: " + out);
}

void CmpTst::CompareTest::clean_up(string ref, string out)
{
	string ref_tmp = ref + TMP_SUFFIX;
	bool ref_err = false, out_err = false;

	ifstream refstream, outstream;
	refstream.open(ref_tmp.c_str());
	if(refstream)
	{
		refstream.close();
		if(remove(ref_tmp.c_str()) != 0)
			ref_err = true;
	}

	outstream.open(out.c_str());
	if(outstream)
	{
		outstream.close();
		if(remove(out.c_str()) != 0)
			out_err = true;
	}

	if(ref_err)
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::clean_up] Cannot remove file: " + ref_tmp);

	if(out_err)
		throw CmpTst::CompareTestException("[CmpTst::CompareTest::clean_up] Cannot remove file: " + out);
}

void CmpTst::CompareTest::print_output(string out)
{
	print_output(out, false);
}

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
