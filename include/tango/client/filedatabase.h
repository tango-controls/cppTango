// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.


#ifndef FILEDATABASE_H
#define FILEDATABASE_H

#include <fstream>
#include <string>
#include <vector>
#include <tango.h>

namespace Tango{

#define _TG_NUMBER  1
#define _TG_STRING  2
#define _TG_COMA    3
#define _TG_COLON   4
#define _TG_SLASH   5
#define _TG_ASLASH  6
#define _TG_ARROW   7



class t_property
{
public:
	std::string 				name;
	std::vector<std::string> 	value;

};


class t_attribute_property
{
public:
	std::string 				attribute_name;
	std::vector<t_property*> 	properties;
};


class t_device
{
public:
	std::string 						name;
	std::vector<t_property*> 			properties;
	std::vector<t_attribute_property*> 	attribute_properties;
};

class t_tango_class
{
public:
	std::string 						name;
	std::string 						description;
	std::string 						title;
	std::vector<t_device*> 				devices;
	std::vector<t_property*> 			properties;
	std::vector<t_attribute_property*> 	attribute_properties;
};


class t_server
{
public:
	std::string 					name;
	std::string 					instance_name;
	std::vector<t_tango_class*> 	classes;
	std::vector<t_device*> 			devices;
};

template <class T> class hasName
{
    string 		name;
public:
    hasName (string _name) : name(_name) {};
    bool operator () (T* obj);
};

template <class T> class hasAttributeName
{
    string 		attribute_name;
public:
    hasAttributeName (string _name) : attribute_name(_name) {};
    bool operator () (T* obj);
};

class FileDatabaseExt
{
public:

	FileDatabaseExt();

	~FileDatabaseExt();

};


class FileDatabase
{
public:
	FileDatabase(const std::string& file_name);
	~FileDatabase();
	std::string  parse_res_file(const std::string& file_name);
	void display();
	string get_display();
	void write_event_channel_ior(string &);


	Tango::Any*      DbInfo(Tango::Any&);
	Tango::Any*      DbImportDevice(Tango::Any&);
	Tango::Any*      DbExportDevice(Tango::Any&);
	Tango::Any*      DbUnExportDevice(Tango::Any&);
	Tango::Any*      DbAddDevice(Tango::Any&);
	Tango::Any*      DbDeleteDevice(Tango::Any&);
	Tango::Any*      DbAddServer(Tango::Any&);
	Tango::Any*      DbDeleteServer(Tango::Any&);
	Tango::Any*      DbExportServer(Tango::Any&);
	Tango::Any*      DbUnExportServer(Tango::Any&);
	Tango::Any*      DbGetServerInfo(Tango::Any&);

	Tango::Any*      DbGetDeviceProperty(Tango::Any&);
	Tango::Any*      DbPutDeviceProperty(Tango::Any&);
	Tango::Any*      DbDeleteDeviceProperty(Tango::Any&);
	Tango::Any*      DbGetDeviceAttributeProperty(Tango::Any&);
	Tango::Any*      DbPutDeviceAttributeProperty(Tango::Any&);
	Tango::Any*      DbDeleteDeviceAttributeProperty(Tango::Any&);
	Tango::Any*      DbGetClassProperty(Tango::Any&);
	Tango::Any*      DbPutClassProperty(Tango::Any&);
	Tango::Any*      DbDeleteClassProperty(Tango::Any&);
	Tango::Any*      DbGetClassAttributeProperty(Tango::Any&);
	Tango::Any*      DbPutClassAttributeProperty(Tango::Any&);
	Tango::Any*      DbDeleteClassAttributeProperty(Tango::Any&);
	Tango::Any*      DbGetDeviceList(Tango::Any&);

	Tango::Any*      DbGetDeviceDomainList(Tango::Any&);
	Tango::Any*      DbGetDeviceMemberList(Tango::Any&);
	Tango::Any*      DbGetDeviceExportedList(Tango::Any&);
	Tango::Any*      DbGetDeviceFamilyList(Tango::Any&);
	Tango::Any*      DbGetProperty(Tango::Any&);
	Tango::Any*      DbPutProperty(Tango::Any&);
	Tango::Any*      DbDeleteProperty(Tango::Any&);
	Tango::Any*      DbGetAliasDevice(Tango::Any&);
	Tango::Any*      DbGetDeviceAlias(Tango::Any&);
	Tango::Any*      DbGetAttributeAlias(Tango::Any&);
	Tango::Any*      DbGetDeviceAliasList(Tango::Any&);
	Tango::Any*      DbGetAttributeAliasList(Tango::Any&);

	Tango::Any*		 DbGetClassPipeProperty(Tango::Any&);
	Tango::Any*		 DbGetDevicePipeProperty(Tango::Any&);
	Tango::Any*		 DbDeleteClassPipeProperty(Tango::Any&);
	Tango::Any*		 DbDeleteDevicePipeProperty(Tango::Any&);
	Tango::Any* 	 DbPutClassPipeProperty(Tango::Any&);
	Tango::Any* 	 DbPutDevicePipeProperty(Tango::Any&);

	void write_file();

private:
	string 			filename;
	t_server 		m_server;

	void read_char(std::ifstream& f);
	int class_lex(std::string& word);
	void  jump_line(std::ifstream& f);
	void  jump_space(std::ifstream& f);
	std::string read_word(std::ifstream& f);
	void CHECK_LEX(int lt,int le);
	std::vector<std::string> parse_resource_value(std::ifstream& f);

	std::string read_full_word(std::ifstream& f);
	void escape_double_quote(string &);


	static const char* lexical_word_null;
	static const char* lexical_word_number;
	static const char* lexical_word_string;
	static const char* lexical_word_coma;
	static const char* lexical_word_colon;
	static const char* lexical_word_slash;
	static const char* lexical_word_backslash;
	static const char* lexical_word_arrow;
	static int ReadBufferSize;
	static int MaxWordLength;


	int 			length_buf;
  	int 			pos_buf;
  	int 			CrtLine;
  	int 			StartLine;
  	char 			CurrentChar;
  	char 			NextChar;

  	bool 			DELETE_ENTRY;
  	string 			word;

	FileDatabaseExt	*ext;
};

} // end namespace Tango

#endif

