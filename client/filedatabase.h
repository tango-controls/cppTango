// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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
	void write_event_channel_ior(string &,string &);


	CORBA::Any*      DbInfo(CORBA::Any&);
	CORBA::Any*      DbImportDevice(CORBA::Any&);
	CORBA::Any*      DbExportDevice(CORBA::Any&);
	CORBA::Any*      DbUnExportDevice(CORBA::Any&);
	CORBA::Any*      DbAddDevice(CORBA::Any&);
	CORBA::Any*      DbDeleteDevice(CORBA::Any&);
	CORBA::Any*      DbAddServer(CORBA::Any&);
	CORBA::Any*      DbDeleteServer(CORBA::Any&);
	CORBA::Any*      DbExportServer(CORBA::Any&);
	CORBA::Any*      DbUnExportServer(CORBA::Any&);
	CORBA::Any*      DbGetServerInfo(CORBA::Any&);

	CORBA::Any*      DbGetDeviceProperty(CORBA::Any&);
	CORBA::Any*      DbPutDeviceProperty(CORBA::Any&);
	CORBA::Any*      DbDeleteDeviceProperty(CORBA::Any&);
	CORBA::Any*      DbGetDeviceAttributeProperty(CORBA::Any&);
	CORBA::Any*      DbPutDeviceAttributeProperty(CORBA::Any&);
	CORBA::Any*      DbDeleteDeviceAttributeProperty(CORBA::Any&);
	CORBA::Any*      DbGetClassProperty(CORBA::Any&);
	CORBA::Any*      DbPutClassProperty(CORBA::Any&);
	CORBA::Any*      DbDeleteClassProperty(CORBA::Any&);
	CORBA::Any*      DbGetClassAttributeProperty(CORBA::Any&);
	CORBA::Any*      DbPutClassAttributeProperty(CORBA::Any&);
	CORBA::Any*      DbDeleteClassAttributeProperty(CORBA::Any&);
	CORBA::Any*      DbGetDeviceList(CORBA::Any&);

	CORBA::Any*      DbGetDeviceDomainList(CORBA::Any&);
	CORBA::Any*      DbGetDeviceMemberList(CORBA::Any&);
	CORBA::Any*      DbGetDeviceExportedList(CORBA::Any&);
	CORBA::Any*      DbGetDeviceFamilyList(CORBA::Any&);
	CORBA::Any*      DbGetProperty(CORBA::Any&);
	CORBA::Any*      DbPutProperty(CORBA::Any&);
	CORBA::Any*      DbDeleteProperty(CORBA::Any&);
	CORBA::Any*      DbGetAliasDevice(CORBA::Any&);
	CORBA::Any*      DbGetDeviceAlias(CORBA::Any&);
	CORBA::Any*      DbGetAttributeAlias(CORBA::Any&);
	CORBA::Any*      DbGetDeviceAliasList(CORBA::Any&);
	CORBA::Any*      DbGetAttributeAliasList(CORBA::Any&);


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
 
