#include <iostream>
#include <tango.h>

// DbInfo                              done
// DbImportDevice
// DbExportDevice
// DbUnExportDevice
// DbAddDevice
// DbDeleteDevice
// DbAddServer
// DbDeleteServer
// DbExportServer
// DbUnExportServer
// DbGetServerInfo


// DbGetDeviceProperty                  done
// DbPutDeviceProperty                  done
// DbDeleteDeviceProperty               done
// DbGetDeviceAttributeProperty         done
// DbPutDeviceAttributeProperty         done 
// DbDeleteDeviceAttributeProperty      to check
// DbGetClassProperty                   done
// DbPutClassProperty                   done
// DbDeleteClassProperty
// DbGetClassAttributeProperty          done 
// DbPutClassAttributeProperty          done
// DbDeleteClassAttributeProperty
// DbGetDeviceList                      done

// DbGetDeviceMemberList
// DbGetDeviceExportedList
// DbGetDeviceDomainList
// DbGetDeviceFamilyList
// DbGetProperty
// DbPutProperty
// DbDeleteProperty
// DbGetAliasDevice
// DbGetDeviceAlias
// DbGetAttributeAlias
// DbGetDeviceAliasList
// DbGetAttributeAliasList

// ****************************************************
// Go to the next significant character
// ****************************************************

// ****************************************************
// return the lexical classe of the next word               */
// ****************************************************

using namespace std;

namespace Tango
{

const char* FileDatabase::lexical_word_null      = "NULL";
const char* FileDatabase::lexical_word_number    = "NUMBER";
const char* FileDatabase::lexical_word_string    = "STRING";
const char* FileDatabase::lexical_word_coma      = "COMA";
const char* FileDatabase::lexical_word_colon     = "COLON";
const char* FileDatabase::lexical_word_slash     = "SLASH";
const char* FileDatabase::lexical_word_backslash = "BackSLASH";
const char* FileDatabase::lexical_word_arrow     = "->";
int FileDatabase::ReadBufferSize=4069;
int FileDatabase::MaxWordLength=256;

char chartolower(const char c)
{
    	if (c >= 'A' && c <= 'Z')
    		return c - 'A' + 'a';
    	else
    		return c;
}

bool equalsIgnoreCase(const string& s1, const string& s2)
{
	bool ret = false;
	if (s1.size() == s2.size())
	{
		int l = s1.size();
		while (l)
		{
			ret = ( chartolower(s1.at(l-1)) == chartolower(s2.at(l-1)) );
			l--;
			if (!ret) break;
		}
	}
  	return ret;
}

t_device* search_device(t_server& s, string& name)
{
	for (unsigned int j = 0; j < s.devices.size(); j++)
	{
		if (equalsIgnoreCase(s.devices[j]->name, name))
			return s.devices[j]; 
	};
	
	return NULL;
}

t_tango_class* search_class(t_server& s, string& name)
{
	for (unsigned int i = 0; i < s.classes.size(); i++)
	{
		if (equalsIgnoreCase(s.classes[i]->name, name))
			return s.classes[i];
				
	}
	return NULL;
}

t_attribute_property* search_dev_attr_prop(t_device* d, const string& name)
{
	for (unsigned int i = 0; i < d->attribute_properties.size(); i++)
	{
		if (equalsIgnoreCase(d->attribute_properties[i]->attribute_name, name))
			return d->attribute_properties[i];
	}
	return NULL;

}

t_attribute_property* search_class_attr_prop(t_tango_class* d, const string& name)
{
	for (unsigned int i = 0; i < d->attribute_properties.size(); i++)
	{
		if (equalsIgnoreCase(d->attribute_properties[i]->attribute_name, name))
			return d->attribute_properties[i];
	}
	return NULL;

}

std::string& trim(string& str)
{  
// trim leading whitespace
   	string::size_type  notwhite = str.find_first_not_of(" \t\n");
   	str.erase(0,notwhite);

// trim trailing whitespace
   	notwhite = str.find_last_not_of(" \t\n"); 
   	str.erase(notwhite+1);
	return str;
} 


std::vector<std::string>& makeStringArray(const std::string& input, vector<string>& results)
{

	std::string delimiter = "\n";
  int iPos = 0;
  int newPos = -1;
  int sizeS2 = delimiter.size();
  int isize = input.size();

  std::vector<int> positions;

  newPos = input.find (delimiter, 0);

  if( newPos < 0 ) { return results; }

  int numFound = 0;

  while( newPos > iPos )
  {
    numFound++;
    positions.push_back(newPos);
    iPos = newPos;
    newPos = input.find (delimiter, iPos+sizeS2+1);
  }

  for(unsigned int i=0; i <= positions.size(); i++ )
  {
    string s;
    if( i == 0 ) { s = input.substr( i, positions[i] ); }
    int offset = positions[i-1] + sizeS2;
    if( offset < isize )
    {
      if( i == positions.size() )
      {
        s = input.substr(offset);
      }
      else if( i > 0 )
      {
        s = input.substr( positions[i-1] + sizeS2, 
          positions[i] - positions[i-1] - sizeS2 );
      }
    }
    if( s.size() > 0 )
    {
      results.push_back(trim(s));
    }
  }
  return results;
}

template <class T>
bool hasName<T>::operator() (T* obj)
{ 
	return (Tango::equalsIgnoreCase(obj->name,name)); 
}

template <class T>
bool hasAttributeName<T>::operator() (T* obj)
{ 
	return (Tango::equalsIgnoreCase(obj->attribute_name,attribute_name)); 
}


FileDatabaseExt::FileDatabaseExt() {}
FileDatabaseExt::~FileDatabaseExt() {}


FileDatabase::FileDatabase(const std::string& file_name)
{
	cout4 << "FILEDATABASE: FileDatabase constructor" << endl;
	filename = file_name;
	
	parse_res_file(filename);
}

FileDatabase::~FileDatabase()
{
	cout4 << "FILEDATABASE: FileDatabase destructor" << endl;
//	write_file();

	std::vector<t_device*>::iterator i;
	for(i = m_server.devices.begin(); i != m_server.devices.end(); i++)
	{
		std::vector<t_property*>::iterator p;
		for(p = (*i)->properties.begin(); p != (*i)->properties.end(); p++)
		{
			delete (*p);
		}
		std::vector<t_attribute_property*>::iterator pa;
		for(pa = (*i)->attribute_properties.begin(); pa != (*i)->attribute_properties.end(); pa++)
		{
			std::vector<t_property*>::iterator p;
			for(p = (*pa)->properties.begin(); p != (*pa)->properties.end(); p++)
			{
				delete (*p);
			}
			delete (*pa);
		}
		delete (*i);
	}
	std::vector<t_tango_class*>::iterator j;
	for(j = m_server.classes.begin(); j != m_server.classes.end(); j++)
	{
		std::vector<t_property*>::iterator p;
		for(p = (*j)->properties.begin(); p != (*j)->properties.end(); p++)
		{
			delete (*p);
		}
		std::vector<t_attribute_property*>::iterator pa;
		for(pa = (*j)->attribute_properties.begin(); pa != (*j)->attribute_properties.end(); pa++)
		{
			std::vector<t_property*>::iterator p;
			for(p = (*pa)->properties.begin(); p != (*pa)->properties.end(); p++)
			{
				delete (*p);
			}
			delete (*pa);
		}
		delete (*j);
	}

}

// ****************************************************
// read the next character in the file
// ****************************************************
void FileDatabase :: read_char(ifstream& f)
{
  	CurrentChar=NextChar;
  	if(!f.eof()) 
		f.get(NextChar);   
  	else         
		NextChar=0;    
  	if(CurrentChar=='\n') 
		CrtLine++;
}

int FileDatabase :: class_lex(string& tmp_word) 
{
/* exepction */
	if(tmp_word.empty()) return 0;
	if(tmp_word.length()==0) return _TG_STRING;
  
/* Special character */

	if(tmp_word == "/")  return _TG_SLASH;
	if(tmp_word == "\\") return _TG_ASLASH;
	if(tmp_word == ",")  return _TG_COMA;
	if(tmp_word == ":")  return _TG_COLON;
	if(tmp_word == "->") return _TG_ARROW;
  
	return _TG_STRING;
}


// ****************************************************
// Go to the next line                                      */
// ****************************************************
void  FileDatabase :: jump_line(ifstream& f) 
{
	while(CurrentChar!='\n' && CurrentChar!=0) read_char(f);
		read_char(f);
}


void  FileDatabase :: jump_space(ifstream& f)
{
	while((CurrentChar<=32) && (CurrentChar>0)) 
		read_char(f);
}

  // ****************************************************
  // Read the next word in the file                           */
  // ****************************************************
string FileDatabase :: read_word(ifstream& f) 
{
 
   string ret_word="";
 
   /* Jump space and comments */
   jump_space(f);
   while( CurrentChar=='#' ) {
     jump_line(f);
     jump_space(f);
   }

   /* Jump C like comments */
   if( CurrentChar=='/' ) {
     read_char(f);
     if( CurrentChar=='*' ) {
       bool end=false;
       read_char(f);
       while(end) {
         while( CurrentChar!='*' )
           read_char(f);
         read_char(f);
         end=(CurrentChar=='/');
       }
       read_char(f);
       jump_space(f);
     } else {
       ret_word="/";
       return ret_word;
     }
   }
   
   StartLine=CrtLine;

   /* Treat special character */
   if( CurrentChar==':' || CurrentChar=='/' || CurrentChar==',' ||
       CurrentChar=='\\' || ( CurrentChar=='-' && NextChar=='>' ) )
   {
     if( CurrentChar!='-' ) {
       ret_word += CurrentChar;
     } else {
       ret_word += CurrentChar;
       read_char(f);
       ret_word += CurrentChar;
     }
     read_char(f);
     return ret_word;
   }

   /* Treat string */
   if( CurrentChar=='"' ) {
     read_char(f);
     while( CurrentChar!='"' && CurrentChar!=0 && CurrentChar!='\n' ) {
       ret_word+=CurrentChar;
       read_char(f);
     }
     if(CurrentChar==0 || CurrentChar=='\n')
     {
		cout3 << "Error at line " << StartLine << endl;
		TangoSys_MemStream desc;
		desc << "File database: Error in file at line " << StartLine;
		desc << " in file " << filename << "." << ends;
		ApiConnExcept::throw_exception((const char *)"API_DatabaseFileError",
				               desc.str(),
				               (const char *)"FileDatabase::CHECK_LEX");
     }
     read_char(f);
     return ret_word;
   }

   /* Treat other word */
   while( CurrentChar>32 && CurrentChar!=':' && CurrentChar!='/' 
	  && CurrentChar!='\\' && CurrentChar!=',' )
   {
     if( CurrentChar=='-' && NextChar=='>' )
       break;     
     ret_word+=CurrentChar;
     read_char(f);
   }

   if(ret_word.length()==0) {
     return string(lexical_word_null);
   }
   
   return ret_word;
  }

  // ****************************************************
  // Read the next word in the file                  
  // And allow / inside                  
  // ****************************************************
string FileDatabase:: read_full_word(ifstream& f)
{
 	string ret_word;

	StartLine=CrtLine;
	jump_space(f);

	/* Treat special character */
	if( CurrentChar==',' || CurrentChar=='\\' )
	{
  		ret_word += CurrentChar;
  		read_char(f);
  		return ret_word;
	}

	/* Treat string */
	if( CurrentChar=='"' )
	{
  		read_char(f);
  		while( CurrentChar!='"' && CurrentChar!=0 && CurrentChar!='\n')
		{
    			ret_word += CurrentChar;
    			read_char(f);
  		}
  		if( CurrentChar==0 || CurrentChar=='\n')
  		{
    			cout3 << "Warning: String too long at line " << StartLine << endl;
			TangoSys_MemStream desc;
			desc << "File database: String too long at line " << StartLine;
			desc << " in file " << filename << "." << ends;
			ApiConnExcept::throw_exception((const char *)"API_DatabaseFileError",
				       desc.str(),
				       (const char *)"FileDatabase::read_full_word");
  		}
  		read_char(f);
  		return ret_word;
	}

	/* Treat other word */
	while( CurrentChar>32 && CurrentChar!='\\' && CurrentChar!=',')
	{
  		ret_word += CurrentChar;
  		read_char(f);
	}

	if(ret_word.length()==0)
	{
		ret_word = string(lexical_word_null);
  		return ret_word;
	}

	return ret_word;
}


void FileDatabase:: CHECK_LEX(int lt,int le) 
{
   	if( lt!=le )
	{
    		cout3 << "Error at line " << StartLine << endl;
		TangoSys_MemStream desc;
		desc << "File database: Error in file at line " << StartLine;
		desc << " in file " << filename << "." << ends;
		ApiConnExcept::throw_exception((const char *)"API_DatabaseFileError",
				               desc.str(),
				               (const char *)"FileDatabase::CHECK_LEX");
	}
}


vector<string> FileDatabase:: parse_resource_value(ifstream& f)
{
	int  lex;
	vector<string> ret;
	int       nbr;
  
	/* Resource value */
	lex=_TG_COMA;
	nbr=0;
  
	while( (lex==_TG_COMA || lex==_TG_ASLASH) && word!="" ) 
	{
		word=read_full_word(f);
		lex=class_lex(word);
 
  	/* allow ... ,\ syntax */
    		if( lex==_TG_ASLASH )
		{
      			word=read_full_word(f);
			lex=class_lex(word);
    		}
     
    		CHECK_LEX(lex,_TG_STRING);

    		ret.push_back(word);
    		nbr++;
     
    		word=read_word(f);
		lex=class_lex(word);
     
  	}
   
	return ret;
   
}

  // ****************************************************
  // Parse a resource file
  // Return error as String (zero length when sucess)
  // ****************************************************


string FileDatabase::parse_res_file(const string& file_name)
{
    	ifstream f;
    	bool eof=false;
    	int lex;

	t_tango_class* un_class;
	t_device* un_device;

	string domain;
	string family;
	string member;
	string name;
    	string prop_name;

    	CrtLine=1;
    	NextChar=' ';
    	CurrentChar=' ';

    	cout4 << "FILEDATABASE: entering parse_res_file" << endl;
     /* OPEN THE FILE                  */

    	f.open (file_name.c_str(), ifstream::in);
    	if ( !f.good() )
    	{
		TangoSys_MemStream desc;
		desc << "FILEDATABASE could not open file " << file_name << "." << ends;
		ApiConnExcept::throw_exception((const char *)"API_DatabaseFileError",
				       desc.str(),
				       (const char *)"FileDatabase::parse_res_file");
    	}

/* CHECK BEGINING OF CONFIG FILE  */

     	word=read_word(f);
     	if( word == "" ) 
	{
		f.close();
        	return file_name + " is empty...";
	}
     	lex=class_lex(word);
	m_server.name = word;
	
/* PARSE                          */

     	while( !eof )
     	{
       		switch(lex)
		{
/* Start a resource mame */
         	case _TG_STRING:
	
/* Domain */
           		domain=word;
           		word=read_word(f);
			lex=class_lex(word);
			//cout << "DOMAIN " << domain << endl;;
           		CHECK_LEX(lex,_TG_SLASH);

/* Family */
           		word=read_word(f);
			lex=class_lex(word);
           		CHECK_LEX(lex,_TG_STRING);
           		family=word;
			//cout << "FAMILI " << family << endl;
           		word=read_word(f);
			lex=class_lex(word);
        
	   		switch(lex)
			{
	   		case _TG_SLASH:

	     /* Member */
             			word=read_word(f);lex=class_lex(word);
             			CHECK_LEX(lex,_TG_STRING);
             			member=word;
             			word=read_word(f);
				lex=class_lex(word);
	
             			switch(lex)
				{
	       			case _TG_SLASH:
	         /* We have a 4 fields name */
           				word=read_word(f);
					lex=class_lex(word);
           				CHECK_LEX(lex,_TG_STRING);
	         			name=word;
					
           				word=read_word(f);
					lex=class_lex(word);
	  
	         			switch(lex)
					{
	  				case _TG_COLON: 
				     		{
	               /* Device definition */
						m_server.instance_name = family;
	               				vector<string> values = parse_resource_value(f);
	               				lex=class_lex(word);
						//cout << "Class name : " << name << endl;
						un_class = new t_tango_class;
						un_class->name = name;
						m_server.classes.push_back(un_class);
	               				if( equalsIgnoreCase(member, "device") )
						{	    
	                 /* Device definition */
							for (unsigned int n = 0; n < values.size(); n++)
							{ 
	                 //cout << "    Device: <" << values[n] << ">" << endl;
								un_device = new t_device;
								un_device->name = values[n];
								m_server.devices.push_back(un_device);
								un_class->devices.push_back(un_device);
							}
		        			}	    
		      				}
	             				break;
	    
	           			case _TG_ARROW:  
	             				{
	               /* We have an attribute property definition */
                       				word=read_word(f);
						lex=class_lex(word);
                       				CHECK_LEX(lex,_TG_STRING);
	                		 	prop_name=word;
						//cout << "Attribute property: " << prop_name << endl;
	    
	               				/* jump : */
                       				word=read_word(f);
						lex=class_lex(word);
                       				CHECK_LEX(lex,_TG_COLON);
	    
	               				/* Resource value */
	               				vector<string> values = parse_resource_value(f);
	               				lex=class_lex(word);
	  
	               /* Device attribute definition */
	               
						//cout << "    " << domain << "/" << family << "/" << member << endl;
						string device_name = domain + "/" + family + "/" + member;
						t_device* d = search_device(m_server, device_name);

						t_attribute_property* un_dev_attr_prop = search_dev_attr_prop(d, name);
						if (un_dev_attr_prop == NULL)
						{
							un_dev_attr_prop = new t_attribute_property;
							un_dev_attr_prop->attribute_name = name;
							d->attribute_properties.push_back(un_dev_attr_prop);
						}
						t_property* prop = new t_property;
						prop->name = prop_name;
						for(unsigned int n = 0; n < values.size(); n++)
						{
							//cout << "     <" << values[n] << ">" << endl;
							prop->value.push_back(values[n]);
						}
						un_dev_attr_prop->properties.push_back(prop);
									
									
	             				}
		    	 			break;
	  
	           			default:  
                     				return "COLON or -> expected at line " + StartLine;
	   
	         			}
	         			break;	    
	    
	       			case _TG_ARROW:
					{
	
	         /* We have a device property or attribute class definition */
            
	        			word=read_word(f);
					lex=class_lex(word);
                			CHECK_LEX(lex,_TG_STRING);
	        			prop_name=word;
	    
	         /* jump : */
                			word=read_word(f); lex=class_lex(word);
                			CHECK_LEX(lex,_TG_COLON);
	    
	         /* Resource value */
	        			vector<string> values = parse_resource_value(f); 
	        			lex=class_lex(word);
	    
	         			if(equalsIgnoreCase(domain, "class"))
					{
	    
	           /* Class attribute property definition */
						 //cout << "Class attribute property definition" << endl;
						 //cout << "      family,member,prop_name,values :" << family <<","<<member<< "," <<prop_name<<","<< endl;
						t_tango_class* c = search_class(m_server, family);
						 
						if (c != NULL)
						{
							t_attribute_property* un_class_attr_prop = search_class_attr_prop(c, member);
							if (un_class_attr_prop == NULL)
							{
								un_class_attr_prop = new t_attribute_property;
								un_class_attr_prop->attribute_name = member;
								c->attribute_properties.push_back(un_class_attr_prop);
							}
							t_property* prop = new t_property;
							prop->name = prop_name;
							for(unsigned int n = 0; n < values.size(); n++)
							{
									//cout << "     <" << values[n] << ">" << endl;
								prop->value.push_back(values[n]);
							}
							un_class_attr_prop->properties.push_back(prop);

	          	 //put_tango_class_attr_prop(family,member,prop_name,values);
						}
	    
	         			}
					else
					{
	      
	           /* Device property definition */
							//cout << "Device property definition " << prop_name << endl;
	            //cout << "    " << domain << "/" << family << "/" << member << endl;
						string device_name = domain + "/" + family + "/" + member;
						t_device* d = search_device(m_server, device_name);

						if (d != NULL)
						{
						 	t_property* un_dev_prop = new t_property;
						 	un_dev_prop->name = prop_name;
						 	for(unsigned int n = 0; n < values.size(); n++)
						 	{
							 //cout << "     <" << values[n] << ">" << endl;
							 	un_dev_prop->value.push_back(values[n]);
						 	}
						 	d->properties.push_back(un_dev_prop);
						}
	         			}
					}
		 			break;
	     	    
	       			default:  
                 			return "SLASH or -> expected at line " + StartLine;
	  
	     			}	  
	     			break;

	   		case _TG_ARROW:
				{
	  
	    	 /* We have a class property */
  	    	 /* Member */
            	 		word=read_word(f); lex=class_lex(word);
            	 		CHECK_LEX(lex,_TG_STRING);
            	 		member=word;
            	 		word=read_word(f); lex=class_lex(word);

	    	 /* Resource value */
	    	 		vector<string> values = parse_resource_value(f); 
  	    	 		lex=class_lex(word);

	    	 /* Class resource */
	    	 		if( equalsIgnoreCase(domain, "class") )
				{	
						//cout << "Tango resource class " << endl;
					{
						un_class = search_class(m_server, family);
						if (un_class != NULL)
						{
							 t_property* un_prop = new t_property;
							 un_prop->name = member;
							 //cout << "Proprieta : " << member << endl;
							 for(unsigned int n = 0; n< values.size(); n++)
							 {
								 //cout << "    " << member << "[" << n << "] = " << values[n] << endl;
								 un_prop->value.push_back(values[n]);
							 }
							 un_class->properties.push_back(un_prop);
						 }
					}   
	      	 
	    	 		}
				else if ( equalsIgnoreCase(domain, "free") )
				{
//						cout << "Free Tango res" << endl;
	      	 //put_free_tango_res(family,member,values);
	    	 		}
				else
				{
	      	 			return "Invlalid class property syntax on " + domain + "/" + family +"/"+ member;
	    	 		}
				}
	     			break;
	  	  
	   		default:
             			return "SLASH or -> expected at line " + StartLine;	   
			}
			break;

      		default:
        		return "Invalid resource name get  instead of STRING al line " + StartLine;
      		}

      		eof=(word == lexical_word_null);
     		}
   
		 f.close();
     	return "";
  
}

void FileDatabase:: display()
{
	cout << " ************************** " << endl;
	cout << "server = " << m_server.name << endl;
	for (unsigned int i = 0; i < m_server.classes.size(); i++)
	{
		unsigned int j;
		cout << "    class = " << m_server.classes[i]->name << endl;
		for (j = 0; j < m_server.classes[i]->devices.size(); j++)
		{
			unsigned int k;
			cout << "        device = " << m_server.classes[i]->devices[j]->name << endl;
			for (k = 0; k < m_server.classes[i]->devices[j]->properties.size(); k++)
			{
				cout << "            proper = " << m_server.classes[i]->devices[j]->properties[k]->name << "  value: " << endl;
				for (unsigned int l = 0; l < m_server.classes[i]->devices[j]->properties[k]->value.size(); l++)
				{
					cout << "                 value[" << l << "] = " << m_server.classes[i]->devices[j]->properties[k]->value[l] << endl;
				}
			}
			for (k = 0; k < m_server.classes[i]->devices[j]->attribute_properties.size(); k++)
			{
				cout << "            attribute  = " << m_server.classes[i]->devices[j]->attribute_properties[k]->attribute_name <<  endl;
				for (unsigned int l = 0; l < m_server.classes[i]->devices[j]->attribute_properties[k]->properties.size(); l++)
				{
					cout << "                 property[" << l << "] = " << m_server.classes[i]->devices[j]->attribute_properties[k]->properties[l]->name << endl;
					for (unsigned int m = 0; m < m_server.classes[i]->devices[j]->attribute_properties[k]->properties[l]->value.size(); m++)
						cout << "                    value[" << m << "] = " << m_server.classes[i]->devices[j]->attribute_properties[k]->properties[l]->value[m] << endl;
				}			
			}
		}
		for (j = 0; j < m_server.classes[i]->properties.size(); j++)
		{
			cout << "        proper = " << m_server.classes[i]->properties[j]->name << "  value: " << endl;
			for (unsigned int l = 0; l < m_server.classes[i]->properties[j]->value.size(); l++)
				{
					cout << "                 value[" << l << "] = " << m_server.classes[i]->properties[j]->value[l] << endl;
				}
		}
	}
}


string FileDatabase :: get_display()
{
	ostringstream ost;
	ost << " ************************** " << endl;
	ost << "server = " << m_server.name << endl;
	for (unsigned int i = 0; i < m_server.classes.size(); i++)
	{
		unsigned int j;
		ost << "    class = " << m_server.classes[i]->name << endl;
		for (j = 0; j < m_server.classes[i]->devices.size(); j++)
		{
			unsigned int k;
			ost << "        device = " << m_server.classes[i]->devices[j]->name << endl;
			for (k = 0; k < m_server.classes[i]->devices[j]->properties.size(); k++)
			{
				ost << "            proper = " << m_server.classes[i]->devices[j]->properties[k]->name << "  value: " << endl;
				for (unsigned int l = 0; l < m_server.classes[i]->devices[j]->properties[k]->value.size(); l++)
				{
					ost << "                 value[" << l << "] = " << m_server.classes[i]->devices[j]->properties[k]->value[l] << endl;
				}
			}
			for (k = 0; k < m_server.classes[i]->devices[j]->attribute_properties.size(); k++)
			{
				ost << "            attribute  = " << m_server.classes[i]->devices[j]->attribute_properties[k]->attribute_name <<  endl;
				for (unsigned int l = 0; l < m_server.classes[i]->devices[j]->attribute_properties[k]->properties.size(); l++)
				{
					ost << "                 property[" << l << "] = " << m_server.classes[i]->devices[j]->attribute_properties[k]->properties[l]->name << endl;
					for (unsigned int m = 0; m < m_server.classes[i]->devices[j]->attribute_properties[k]->properties[l]->value.size(); m++)
						ost << "                    value[" << m << "] = " << m_server.classes[i]->devices[j]->attribute_properties[k]->properties[l]->value[m] << endl;
				}			
			}
		}
		for (j = 0; j < m_server.classes[i]->properties.size(); j++)
		{
			ost << "        proper = " << m_server.classes[i]->properties[j]->name << "  value: " << endl;
			for (unsigned int l = 0; l < m_server.classes[i]->properties[j]->value.size(); l++)
				{
					ost << "                 value[" << l << "] = " << m_server.classes[i]->properties[j]->value[l] << endl;
				}
		}
	}
	return ost.str();
}

void  FileDatabase :: write_file()
{
	ofstream f;
	string f_name(filename);
	/*
	string::size_type pos;
	if ((pos = f_name.rfind('/')) == string::npos)
		f_name = "_" + f_name;
	else
		f_name.insert(pos + 1,"_",1);
	*/
	
	f.open (f_name.c_str());
	vector<t_tango_class *>::const_iterator it;
	for(it = m_server.classes.begin(); it != m_server.classes.end(); ++it)
	{
		f << m_server.name << "/" << m_server.instance_name << "/DEVICE/" <<  (*it)->name << ": " ;
		int margin = m_server.name.size() + 1 + m_server.instance_name.size() + 8 + (*it)->name.size() + 2;
		string margin_s(margin,' ');
		vector<t_device*>::iterator iterator_d = (*it)->devices.begin();
		f << "\"" << (*iterator_d)->name << "\"";
		++iterator_d;
		for(vector<t_device*>::iterator itd = iterator_d; itd != (*it)->devices.end(); ++itd)
		{
			f << ",\\" << endl;
			f << margin_s << "\"" << (*itd)->name << "\"";
		}
		f << endl;
	}
	f << endl;
	
	for(it = m_server.classes.begin(); it != m_server.classes.end(); ++it)
	{
		f << "#############################################" << endl;
		f << "# CLASS " << (*it)->name << endl;
		f << endl;
		for(vector<t_property*>::iterator itp = (*it)->properties.begin(); itp != (*it)->properties.end(); ++itp)
		{
			f << "CLASS/" << (*it)->name << "->" << (*itp)->name << ": " ;
			int margin = 6 + (*it)->name.size() + 2 + (*itp)->name.size() + 2;
			string margin_s(margin,' ');
			vector<string>::iterator iterator_s = (*itp)->value.begin();
			if (iterator_s != (*itp)->value.end())
			{
				//f << "\"" << (*iterator_s) << "\"";
				if (iterator_s->find(' ', 0)!=string::npos) 
						f << "\"" ;
					f << (*iterator_s);
				if (iterator_s->find(' ', 0)!=string::npos) 
						f << "\"";
				++iterator_s;
				for(vector<string>::iterator its = iterator_s; its != (*itp)->value.end(); ++its)
				{
					f << ",\\" << endl;
					f << margin_s;
					if (its->find(' ', 0)!=string::npos) 
						f << "\"" ;
					f << (*its);
					if (its->find(' ', 0)!=string::npos) 
						f << "\"";
				}
			}
		f << endl;
		}
		f << endl;
		f << "# CLASS " << (*it)->name << " attribute properties" << endl;
		f << endl;
		for(vector<t_attribute_property*>::const_iterator itap = (*it)->attribute_properties.begin(); itap != (*it)->attribute_properties.end(); ++itap)
		{
			for(vector<t_property*>::const_iterator itp = (*itap)->properties.begin(); itp != (*itap)->properties.end(); ++itp)
			{
				f << "CLASS/" << (*it)->name << "/" << (*itap)->attribute_name << "->" << (*itp)->name << ": ";
				int margin = 6 + (*it)->name.size() + 1 + (*itap)->attribute_name.size() + 2 + (*itp)->name.size() +2;
				vector<string>::iterator iterator_s = (*itp)->value.begin();
				if (iterator_s != (*itp)->value.end())
				{
					if (iterator_s->find(' ', 0)!=string::npos) 
						f << "\"" ;
					f << (*iterator_s);
					if (iterator_s->find(' ', 0)!=string::npos) 
						f << "\"";
					++iterator_s;
					for(vector<string>::iterator its = iterator_s; its != (*itp)->value.end(); ++its)
					{
						f << ",\\" << endl;
						string margin_s(margin,' ');
						f << margin_s;
						if (its->find(' ', 0)!=string::npos) 
							f << "\"" ;
						f << (*its);
						if (its->find(' ', 0)!=string::npos) 
							f << "\"";
					}
				}
				f << endl;
			}
		}
		f << endl;
		
	}
	f << endl;
	for(vector<t_device*>::const_iterator ite = m_server.devices.begin(); ite != m_server.devices.end(); ++ite)
	{
		f << "# DEVICE " << (*ite)->name << " properties " << endl << endl;
		for(vector<t_property*>::const_iterator itp = (*ite)->properties.begin(); itp != (*ite)->properties.end(); ++itp)
		{
			f << (*ite)->name << "->" << (*itp)->name << ": ";
			vector<string>::iterator iterator_s = (*itp)->value.begin();
			if (iterator_s != (*itp)->value.end())
			{
				int margin = (*ite)->name.size() + 1 + (*itp)->name.size() + 2;
				if (iterator_s->find(' ', 0)!=string::npos) 
					f << "\"" ;
				f << (*iterator_s);
				if (iterator_s->find(' ', 0)!=string::npos) 
					f << "\"";
				++iterator_s;
				for(vector<string>::iterator its = iterator_s; its != (*itp)->value.end(); ++its)
				{
					f << ",\\" << endl;
					string margin_s(margin,' ');
					f << margin_s;
					if (its->find(' ', 0)!=string::npos) 
						f << "\"" ;
					f << (*its);
					if (its->find(' ', 0)!=string::npos) 
						f << "\"";
				}
			}
			f << endl;
		}
		f << endl;
		f << "# DEVICE " <<  (*ite)->name << " attribute properties" << endl << endl;
		for(vector<t_attribute_property*>::const_iterator itap = (*ite)->attribute_properties.begin(); itap != (*ite)->attribute_properties.end(); ++itap)
		{
			for(vector<t_property*>::const_iterator itp = (*itap)->properties.begin(); itp != (*itap)->properties.end(); ++itp)
			{
				f << (*ite)->name << "/" << (*itap)->attribute_name << "->" << (*itp)->name << ": ";
				int margin = (*ite)->name.size() + 1 + (*itap)->attribute_name.size() + 2 + (*itp)->name.size() +2;
				vector<string>::iterator iterator_s = (*itp)->value.begin();
				if (iterator_s != (*itp)->value.end())
				{
					if (iterator_s->find(' ', 0)!=string::npos) 
						f << "\"" ;
					f << (*iterator_s);
					if (iterator_s->find(' ', 0)!=string::npos) 
						f << "\"";
					++iterator_s;
					for(vector<string>::iterator its = iterator_s; its != (*itp)->value.end(); ++its)
					{
						f << ",\\" << endl;
						string margin_s(margin,' ');
						f << margin_s;
						if (its->find(' ', 0)!=string::npos) 
							f << "\"" ;
						f << (*its);
						if (its->find(' ', 0)!=string::npos) 
							f << "\"";
					}
				}
				f << endl;
			}
		}
		
	}

	f.close();
}


CORBA::Any*   FileDatabase :: DbGetDeviceProperty(CORBA::Any& send)
{ 
	Tango::DevVarStringArray* data_out = new DevVarStringArray;
	const Tango::DevVarStringArray* data_in;
	char num_prop_str[256];
	char num_vals_str[256];
	unsigned int num_prop = 0;
	int num_val  = 0;

	cout4 << "FILEDATABASE: entering DbGetDeviceProperty" << endl;

	send >>= data_in;

	CORBA::Any* any_ptr;
	any_ptr = new CORBA::Any();	
	int index = 0;
	int seq_length = 2;
		
	data_out->length(2);
	(*data_out)[0] = CORBA::string_dup( (*data_in)[0] ); index++;
	num_prop = data_in->length() - 1;
	sprintf(num_prop_str,"%d",num_prop);	
	(*data_out)[index] = CORBA::string_dup(num_prop_str); index++;
	
	if (data_in->length() >= 2)
	{
		unsigned long nb_defined_dev = m_server.devices.size();
		unsigned long i;
		for(i = 0; i < nb_defined_dev; i++)
		{
			if ( equalsIgnoreCase((*data_in)[0].in(), m_server.devices[i]->name))
			{
				for (unsigned int j = 1; j < data_in->length(); j++)
				{
					unsigned long m;
					unsigned long nb_defined_prop = m_server.devices[i]->properties.size();
					for (m = 0; m < nb_defined_prop; m++)
					{
						//if ( strcmp((*data_in)[j], m_server.devices[i]->properties[m]->name.c_str()) == 0 )
						if ( equalsIgnoreCase((*data_in)[j].in(), m_server.devices[i]->properties[m]->name))
						{
							num_prop++;
							num_val = m_server.devices[i]->properties[m]->value.size();
							seq_length = seq_length + 2 + m_server.devices[i]->properties[m]->value.size();
							data_out->length(seq_length);
							(*data_out)[index] = CORBA::string_dup( m_server.devices[i]->properties[m]->name.c_str() );index++;
							sprintf(num_vals_str,"%d",num_val);
							(*data_out)[index] = CORBA::string_dup(num_vals_str); index++;
							for (int k=0; k < num_val; k++)
							{
								(*data_out)[index] = CORBA::string_dup( m_server.devices[i]->properties[m]->value[k].c_str());index++; 
							}
							break;
						}
						
					}
					
					if (m == nb_defined_prop)
					{
						seq_length = seq_length + 3;
						data_out->length(seq_length);
						(*data_out)[index] = CORBA::string_dup((*data_in)[j].in());index++;
						(*data_out)[index] = CORBA::string_dup("0");index++;
						(*data_out)[index] = CORBA::string_dup(" ");index++;
					}
				}
				break;
			}
		}
		
		if (i == nb_defined_dev)
		{
			for (i = 0;i < num_prop;i++)
			{
				seq_length = seq_length + 3;
				data_out->length(seq_length);
				(*data_out)[index] = CORBA::string_dup((*data_in)[i + 1].in());index++;
				(*data_out)[index] = CORBA::string_dup("0");index++;
				(*data_out)[index] = CORBA::string_dup(" ");index++;
			}
		}
	}

	(*any_ptr) <<= data_out;

	//for (unsigned int i = 0; i < data_out->length(); i++)
	//	cout << "data_out[" << i << "] = " << (*data_out)[i] << endl; 

	return any_ptr;
};

CORBA::Any*   FileDatabase :: DbPutDeviceProperty(CORBA::Any& send)
{ 
	cout4 << "FILEDATABASE: entering DbPutDeviceProperty" << endl;

	CORBA::Any* any_ptr = new CORBA::Any;
	
	const Tango::DevVarStringArray* data_in;
	unsigned int n_properties=0;
	int n_values=0;
	int index = 0;
	
	
	send >>= data_in;

	if ((*data_in).length() > 1)
	{

		std::vector<t_device*>::iterator it;
		it = find_if(m_server.devices.begin(), m_server.devices.end(), hasName<t_device>(string((*data_in)[index])));index++;
		if (it == m_server.devices.end())
		{
			cout4 << "Nome device " << (*data_in)[0] << " non trovato. " << endl;
			return any_ptr;
		}
		t_device& device_trovato = *(*(it));

		sscanf((*data_in)[1],"%d",&n_properties); index++;
		for (unsigned int i=0; i< n_properties; i++)
		{
			std::vector<t_property*>::iterator prop_it;
			prop_it = find_if(device_trovato.properties.begin(), device_trovato.properties.end(), hasName<t_property>(string((*data_in)[index])));index++;
			if (prop_it != device_trovato.properties.end())
			{
				/* we found a  property */
				t_property& prop = (*(*prop_it));
				sscanf((*data_in)[index],"%d",&n_values); index++;
				prop.value.resize(n_values);
				for(int j = 0; j< n_values; j++)
				{
					prop.value[j] = (*data_in)[index]; index++;
				}
			} else 
			{
				/* it is a new property */
				t_property* temp_property = new t_property;
				temp_property->name = (*data_in)[index-1]; 
				sscanf((*data_in)[index],"%d",&n_values); index++;
				for(int j = 0; j < n_values; j++)
				{
					temp_property->value.push_back(string((*data_in)[index])); index++;
				}
				device_trovato.properties.push_back(temp_property);
			}
			
		}
	}
	
	
	write_file();
	return any_ptr;

};

CORBA::Any*   FileDatabase :: DbDeleteDeviceProperty(CORBA::Any& send)
{ 
	cout4 << "FILEDATABASE: entering DbDeleteDeviceProperty" << endl;
	
	const Tango::DevVarStringArray* data_in;
		
	send >>= data_in;
	
	//for(unsigned int i = 0; i < (*data_in).length(); i++)
	//	cout << "(*data_in)[" << i << "] = " << (*data_in)[i] << endl; 

	std::vector<t_device*>::iterator it;
	it = find_if(m_server.devices.begin(), m_server.devices.end(), hasName<t_device>(string((*data_in)[0])));

	if (it != m_server.devices.end())
	{
		for(unsigned int i = 1; i < (*data_in).length(); i++)
		{
			t_device& device_trovato = *(*(it));
			std::vector<t_property*>::iterator itp;
			itp = find_if(device_trovato.properties.begin(), device_trovato.properties.end(), hasName<t_property>(string((*data_in)[i])));

			if (itp != device_trovato.properties.end())
			{
				//cout << "found " << (*itp)->name << endl;
				device_trovato.properties.erase(itp, itp+1);
			}
		}
	}

	CORBA::Any* any_ptr = new CORBA::Any;
		
	write_file();
	return any_ptr;
	
};


CORBA::Any*   FileDatabase :: DbGetDeviceAttributeProperty(CORBA::Any& send)
{ 
	Tango::DevVarStringArray* data_out = new DevVarStringArray;
	const Tango::DevVarStringArray* data_in;
	char num_prop_str[256];
	char num_attr_str[256];
	unsigned int num_prop = 0;
	unsigned int num_attr = 0;
	int num_attr_find = 0;

	CORBA::Any* any_ptr;
	any_ptr = new CORBA::Any();	

	cout4 << "FILEDATABASE: entering DbGetDeviceAttributeProperty" << endl;

	send >>= data_in;

	//for(unsigned int i = 0; i < data_in->length(); i++)
	//	cout << "send[" << i << "] = " << (*data_in)[i] << endl;

	int index = 0;
	data_out->length(2);
	(*data_out)[0] = CORBA::string_dup( (*data_in)[0] ); index++;
	num_attr = data_in->length() - 1;
	sprintf(num_attr_str, "%d", num_attr);
	(*data_out)[index] = CORBA::string_dup( num_attr_str ); index++;
	

	std::vector<t_device*>::iterator dev_it;
	dev_it = find_if(m_server.devices.begin(), m_server.devices.end(), hasName<t_device>(string((*data_in)[0])));
	if (dev_it != m_server.devices.end())
	{
		 //cout << "Device " << (*dev_it)->name << " trovato." << endl;
		 for(unsigned int k =0; k < num_attr; k++)
		 {			 
			 data_out->length(index+2);
			(*data_out)[index] = CORBA::string_dup((*data_in)[k+1]); index++; // attribute name
			(*data_out)[index] = CORBA::string_dup("0"); index++; // number of properties
			 for(unsigned int j = 0; j < (*dev_it)->attribute_properties.size(); j++)
			 {
				if (equalsIgnoreCase((*dev_it)->attribute_properties[j]->attribute_name, (*data_in)[k+1].in())) 
				{
					//cout << "Proprieta' " << (*dev_it)->attribute_properties[j]->attribute_name << " trovata." << endl;
					num_prop = (*dev_it)->attribute_properties[j]->properties.size();
					sprintf(num_prop_str, "%d", num_prop);
					//cout << "num proprieta'= " << num_prop_str << endl;
					num_attr_find++;

					(*data_out)[index-1] = CORBA::string_dup(num_prop_str);
					
					for (unsigned int l = 0; l < num_prop; l++)
					{
						char num_val_str[256];
						data_out->length(index + 1 + 1 + (*dev_it)->attribute_properties[j]->properties[l]->value.size());
						(*data_out)[index] = CORBA::string_dup((*dev_it)->attribute_properties[j]->properties[l]->name.c_str());index++;
						sprintf(num_val_str, "%d", (*dev_it)->attribute_properties[j]->properties[l]->value.size());
						(*data_out)[index] = CORBA::string_dup(num_val_str); index++;
						
						for(unsigned int ii = 0; ii < (*dev_it)->attribute_properties[j]->properties[l]->value.size(); ii++)
						{
							//cout << ii << " = " << (*dev_it)->attribute_properties[j]->properties[l]->value[ii].c_str() << endl; 
							(*data_out)[index] = CORBA::string_dup((*dev_it)->attribute_properties[j]->properties[l]->value[ii].c_str()); index++;
						}
					}
					
			  }
				 				
			 }
			}
	} else {
		 data_out->length(index + 2 * num_attr);
		 for(unsigned int i = 0; i < num_attr; i++)
		 {
			(*data_out)[index] = CORBA::string_dup((*data_in)[i+1]); index++;
			(*data_out)[index] = CORBA::string_dup("0"); index++;
		 }

	}
	
	//for(unsigned int i = 0; i < data_out->length(); i++)
	//	cout << "data_out[" << i << "] = " << (*data_out)[i] << endl;
	
	(*any_ptr) <<= data_out;

	return any_ptr;

};


CORBA::Any*   FileDatabase :: DbPutDeviceAttributeProperty(CORBA::Any& send)
{ 
	const Tango::DevVarStringArray* data_in;
	unsigned int num_prop = 0;
	unsigned int num_attr = 0;
	unsigned int num_vals = 0;
	
	CORBA::Any* ret = new CORBA::Any;	
	
	cout4 << "FILEDATABASE: entering DbPutDeviceAttributeProperty" << endl;

	send >>= data_in;


	unsigned int index = 0;
	
	std::vector<t_device*>::iterator dev_it;
	dev_it = find_if(m_server.devices.begin(), m_server.devices.end(), hasName<t_device>(string((*data_in)[index])));index++;
	if (dev_it != m_server.devices.end())
	{
		sscanf((*data_in)[index],"%d",&num_attr); index++;
		for(unsigned int j = 0; j < num_attr; j++)
		{
			t_attribute_property* temp_attribute_property;
			std::vector<t_attribute_property*>::iterator attr_prop_it;
			attr_prop_it = find_if((*dev_it)->attribute_properties.begin(),(*dev_it)->attribute_properties.end(), hasAttributeName<t_attribute_property>(string((*data_in)[index])));
			if (attr_prop_it != (*dev_it)->attribute_properties.end())
			{
				temp_attribute_property = (*attr_prop_it);
			} else {
				// the property is not yet in the file: we add it
				temp_attribute_property = new t_attribute_property;
				temp_attribute_property->attribute_name = string((*data_in)[index]);
				(*dev_it)->attribute_properties.push_back(temp_attribute_property); 
				
			}
			//if (equalsIgnoreCase((*dev_it)->attribute_properties[j]->attribute_name, (*data_in)[index].in())) 
			
			index++;
			sscanf((*data_in)[index],"%d",&num_prop); index++; 
			for (unsigned int i = 0; i < num_prop; i++)
			{
				bool exist = false;
				for(unsigned int k = 0; k < temp_attribute_property->properties.size(); k++)
				{
		  		if (equalsIgnoreCase(temp_attribute_property->properties[k]->name, (*data_in)[index].in())) 
		  		{
						index++;
						temp_attribute_property->properties[k]->value.erase(temp_attribute_property->properties[k]->value.begin(),
                                                        temp_attribute_property->properties[k]->value.begin()+temp_attribute_property->properties[k]->value.size());
						sscanf((*data_in)[index],"%d",&num_vals); index++;
						for(unsigned int n = 0; n < num_vals; n++)
						{
							temp_attribute_property->properties[k]->value.push_back(string((*data_in)[index])); index++;
						}

						//(*dev_it)->attribute_properties[j]->properties[k]->value.push_back( string((*data_in)[index]) );index++;
						if (index >= data_in->length())
						{
							write_file();
							return ret;
						}
						exist = true;
					}
				}
				if ( !exist )
				{
					t_property* new_prop = new t_property;
					new_prop->name = (*data_in)[index]; index++;

					sscanf((*data_in)[index],"%d",&num_vals); index++;
					for(unsigned int n = 0; n < num_vals; n++)
					{
						new_prop->value.push_back(string((*data_in)[index])); index++;
					}

					//makeStringArray( string((*data_in)[index]), new_prop->value);index++;
					//new_prop->value.push_back( string((*data_in)[index]) );index++;
					temp_attribute_property->properties.push_back(new_prop);
					if (index >= data_in->length())
					{
							write_file();
							return ret;
					}
				}

			 } 
		}
			
	} 
	write_file();
	return ret;
};


CORBA::Any*   FileDatabase :: DbDeleteDeviceAttributeProperty(CORBA::Any& send)
{ 
	cout4 << "FILEDATABASE: entering DbDeleteDeviceAttributeProperty" << endl;
	
	const Tango::DevVarStringArray* data_in;
		
	send >>= data_in;
	
	//for(unsigned int i = 0; i < (*data_in).length(); i++)
	//	cout << "(*data_in)[" << i << "] = " << (*data_in)[i] << endl; 

	std::vector<t_device*>::iterator it;
	it = find_if(m_server.devices.begin(), m_server.devices.end(), hasName<t_device>(string((*data_in)[0])));
	std::string attribute_name = string((*data_in)[0]);
	
	if (it != m_server.devices.end())
	{
		t_device& device_trovato = *(*(it));
		for(unsigned int j = 0; j < device_trovato.attribute_properties.size(); j++)
		{
			if (equalsIgnoreCase(device_trovato.attribute_properties[j]->attribute_name, (*data_in)[1].in()))
			{
				for(unsigned int m = 2; m < (*data_in).length(); m++)
				{
					std::vector<t_property*>::iterator itp;
					itp = find_if(device_trovato.attribute_properties[j]->properties.begin(), device_trovato.attribute_properties[j]->properties.end(), hasName<t_property>(string((*data_in)[m])));
					
					if (itp != device_trovato.attribute_properties[j]->properties.end())
					{
//						cout << "found property" << (*itp)->name << "for attribute " << device_trovato.attribute_properties[j]->attribute_name << endl;
						device_trovato.attribute_properties[j]->properties.erase(itp, itp+1);
					}
				}
			}
		}
	}


	CORBA::Any* ret = new CORBA::Any;
	write_file();
	return ret;
	
};


CORBA::Any*   FileDatabase :: DbGetClassProperty(CORBA::Any& send)
{ 
	Tango::DevVarStringArray* data_out = new DevVarStringArray;
	const Tango::DevVarStringArray* data_in;
	char num_prop_str[256];
	char num_vals_str[256];
	unsigned int num_prop = 0;
	unsigned int num_val = 0;

	cout4 << "FILEDATABASE: entering DbGetClassProperty" << endl;

	send >>= data_in;

	CORBA::Any *any_ptr = new CORBA::Any();	
	int index = 0;
	int seq_length = 2;

	
	data_out->length(2);
	(*data_out)[0] = CORBA::string_dup((*data_in)[0]); index++;
	num_prop = data_in->length() - 1;
	sprintf(num_prop_str,"%d",num_prop);	
	(*data_out)[index] = CORBA::string_dup(num_prop_str); index++;
	
	unsigned long nb_classes_defined = m_server.classes.size();
	unsigned long i;
	
	for(i = 0; i < nb_classes_defined; i++)
	{
		if (equalsIgnoreCase((*data_in)[0].in(), m_server.classes[i]->name))
		{
			// m_server.classes[i] e' la classe che cerchiamo
			for (unsigned int j = 1; j < (*data_in).length(); j++)  // in 0 c'e' il nome della classe e poi a seguire le proprieta
			{
				unsigned long nb_prop_defined = m_server.classes[i]->properties.size();
				unsigned long m;
				for (m = 0; m < nb_prop_defined; m++)
				{
					if (equalsIgnoreCase((*data_in)[j].in(), m_server.classes[i]->properties[m]->name))
					{
						num_prop++;
						num_val = m_server.classes[i]->properties[m]->value.size();
						seq_length = seq_length + 2 + num_val;
						(*data_out).length(seq_length);
						(*data_out)[index] = CORBA::string_dup((*data_in)[j]); index++;
						sprintf(num_vals_str,"%d",m_server.classes[i]->properties[m]->value.size());
						(*data_out)[index] = CORBA::string_dup(num_vals_str); index++;
						for (unsigned int n = 0; n < num_val; n++)
						{
							(*data_out)[index] = CORBA::string_dup(m_server.classes[i]->properties[m]->value[n].c_str()); index++;
						}
						break;
					}
				}
				
				if (m == nb_prop_defined)
				{
					seq_length = seq_length + 2;
					data_out->length(seq_length);
					(*data_out)[index] = CORBA::string_dup((*data_in)[i + 1].in());index++;
					(*data_out)[index] = CORBA::string_dup("0");index++;
//					(*data_out)[index] = CORBA::string_dup(" ");index++;
				}
			}
			break;
		}
	}

	if (i == nb_classes_defined)
	{
		for (i = 0;i < num_prop;i++)
		{
			seq_length = seq_length + 2;
			data_out->length(seq_length);
			(*data_out)[index] = CORBA::string_dup((*data_in)[i + 1].in());index++;
			(*data_out)[index] = CORBA::string_dup("0");index++;
//			(*data_out)[index] = CORBA::string_dup(" ");index++;
		}
	}


	(*any_ptr) <<= data_out;

	cout4 << "FILEDATABASE: ending DbGetClassProperty" << endl;
		
	return any_ptr;
};


CORBA::Any*   FileDatabase :: DbPutClassProperty(CORBA::Any& send)
{ 
	CORBA::Any* ret = new CORBA::Any;
	const Tango::DevVarStringArray* data_in;
	unsigned int n_properties=0;
	int n_values=0;
	unsigned int index = 0;
	
	cout4 << "FILEDATABASE: entering DbPutClassProperty" << endl;

	send >>= data_in;
	//for(unsigned int n = 0; n < data_in->length(); n++)
	//	cout << string((*data_in)[n]) << endl;

	if ((*data_in).length() > 1)
	{

		std::vector<t_tango_class*>::iterator it;
		it = find_if(m_server.classes.begin(), m_server.classes.end(), hasName<t_tango_class>(string((*data_in)[index])));index++;
		if (it == m_server.classes.end())
		{
			cout4 << "Nome classe " << (*data_in)[0] << " non trovato. " << endl;
			return ret;
		}
		t_tango_class& classe_trovata = *(*(it));
		
		sscanf((*data_in)[index],"%d",&n_properties); index++;
		for (unsigned int i=0; i< n_properties; i++)
		{
			std::vector<t_property*>::iterator prop_it;
			prop_it = find_if(classe_trovata.properties.begin(), classe_trovata.properties.end(), hasName<t_property>(string((*data_in)[index])));
			if (prop_it != classe_trovata.properties.end())
			{
				/* we found a  property */
				index++;
				t_property& prop = (*(*prop_it));
				sscanf((*data_in)[index],"%d",&n_values); index++;
				prop.value.resize(n_values);
				for(int j = 0; j< n_values; j++)
				{
					prop.value[j] = (*data_in)[index]; index++;
					//db_data[i] >> prop.value; 
				}
			} else 
			{
				/* it is a new property */
				t_property* temp_property = new t_property;
				temp_property->name = (*data_in)[index]; index++;
				sscanf((*data_in)[index],"%d",&n_values); index++;
				for(int j = 0; j < n_values; j++)
				{
					temp_property->value.push_back(string((*data_in)[index])); index++;
				}
				classe_trovata.properties.push_back(temp_property);
				if (index >= data_in->length())
				{
					write_file();
					return ret;
				}
			}
			
		}
	}
	
	write_file();
	return ret;
};


CORBA::Any*   FileDatabase :: DbDeleteClassProperty(CORBA::Any& send)
{ 
	cout4 << "FILEDATABASE: entering DbDeleteClassProperty" << endl;
	
	const Tango::DevVarStringArray* data_in;
		
	send >>= data_in;
	
//	for(unsigned int i = 0; i < (*data_in).length(); i++)
//		cout << "(*data_in)[" << i << "] = " << (*data_in)[i] << endl; 

	std::vector<t_tango_class*>::iterator it;
	it = find_if(m_server.classes.begin(), m_server.classes.end(), hasName<t_tango_class>(string((*data_in)[0])));

	if (it != m_server.classes.end())
	{
		for(unsigned int i = 1; i < (*data_in).length(); i++)
		{
			t_tango_class& classe_trovata = *(*(it));
			std::vector<t_property*>::iterator itp;
			itp = find_if(classe_trovata.properties.begin(), classe_trovata.properties.end(), hasName<t_property>(string((*data_in)[i])));

			if (itp != classe_trovata.properties.end())
			{
				//cout << "found " << (*itp)->name << endl;
				classe_trovata.properties.erase(itp, itp+1);
			}
		}
	}

	CORBA::Any* ret = new CORBA::Any;
	write_file();
	return ret;
};


CORBA::Any*   FileDatabase :: DbGetClassAttributeProperty(CORBA::Any& send)
{ 
	
	CORBA::Any* any_ptr = new CORBA::Any();	
	Tango::DevVarStringArray* data_out = new DevVarStringArray;
	const Tango::DevVarStringArray* data_in;
	unsigned int num_attr  = 0;
	int num_prop = 0;
	char num_attr_str[256];
	char num_prop_str[256];

	cout4 << "FILEDATABASE: entering DbGetClassAttributeProperty" << endl;

	send >>= data_in;

	int index = 0;
	data_out->length(2);
	(*data_out)[0] = CORBA::string_dup((*data_in)[0]); index++;
	num_attr = data_in->length() - 1;
	sprintf(num_attr_str,"%d",num_attr);
	(*data_out)[1] = CORBA::string_dup(num_attr_str); index++;	

	std::vector<t_tango_class*>::iterator it;
	it = find_if(m_server.classes.begin(), m_server.classes.end(), hasName<t_tango_class>(string((*data_in)[0])));
	if (it == m_server.classes.end())
	{
		cout4 << "Nome classe " << (*data_in)[0] << " non trovato. " << endl;
		data_out->length(index + num_attr*2);
		for(unsigned int j = 0; j < num_attr; j++)
		{
			(*data_out)[index] =  CORBA::string_dup((*data_in)[j+1]); index++;
			(*data_out)[index] =  CORBA::string_dup("0"); index++;
		}
		(*any_ptr) <<= data_out;

		return any_ptr;
	}
	t_tango_class& classe_trovata = *(*(it));
	
	for(unsigned int k =0; k < num_attr; k++)
	{	
		data_out->length(index+2);
		(*data_out)[index] = CORBA::string_dup((*data_in)[k+1]); index++;
		(*data_out)[index] = CORBA::string_dup("0"); index++;
			 
		for(unsigned int j = 0; j < classe_trovata.attribute_properties.size(); j++)
		{
		 if (equalsIgnoreCase(classe_trovata.attribute_properties[j]->attribute_name, (*data_in)[k+1].in())) 
		 {
			 num_prop = classe_trovata.attribute_properties[j]->properties.size();
			 sprintf(num_prop_str, "%d", num_prop);
			 //data_out->length(index + 2*num_prop);
			 (*data_out)[index-1] = CORBA::string_dup(num_prop_str);
			 for (unsigned int l = 0; l < classe_trovata.attribute_properties[j]->properties.size(); l++)
			 {
					char num_val_str[256];
					data_out->length(index + 1 + 1 + classe_trovata.attribute_properties[j]->properties[l]->value.size());
					(*data_out)[index] = CORBA::string_dup(classe_trovata.attribute_properties[j]->properties[l]->name.c_str());index++;
					sprintf(num_val_str, "%d", classe_trovata.attribute_properties[j]->properties[l]->value.size());
					(*data_out)[index] = CORBA::string_dup(num_val_str); index++;
				 //(*data_out)[index] = CORBA::string_dup(classe_trovata.attribute_properties[j]->properties[l]->name.c_str());index++;
				 //string temp_value("");
				 if (classe_trovata.attribute_properties[j]->properties[l]->value.size() > 0)
				 {
					 //temp_value += classe_trovata.attribute_properties[j]->properties[l]->value[0];
					 for (unsigned int m = 0; m < classe_trovata.attribute_properties[j]->properties[l]->value.size(); m++)
					 {
						 (*data_out)[index] = CORBA::string_dup(classe_trovata.attribute_properties[j]->properties[l]->value[m].c_str()); index++;
						 //temp_value +=  "\n" + classe_trovata.attribute_properties[j]->properties[l]->value[m];
					 }
				 }
			 }

		 }

		}
	 }
	
	//for(unsigned int i = 0; i < data_out->length(); i++)
	//	cout << "data_out[" << i << "] = " << (*data_out)[i] << endl;	
	
	(*any_ptr) <<= data_out;
		
	return any_ptr;
};


CORBA::Any*  FileDatabase :: DbPutClassAttributeProperty(CORBA::Any& send)
{ 
	CORBA::Any* ret = new CORBA::Any();	
	const Tango::DevVarStringArray* data_in;
	unsigned int num_attr  = 0;
	unsigned int num_prop = 0;
	unsigned int num_vals = 0;
	unsigned int index = 0;

	cout4 << "FILEDATABASE: entering DbPutClassAttributeProperty" << endl;

	send >>= data_in;
	
	std::vector<t_tango_class*>::iterator it;
	it = find_if(m_server.classes.begin(), m_server.classes.end(), hasName<t_tango_class>(string((*data_in)[index])));index++;
	if (it == m_server.classes.end())
	{
		cout4 << "FILEDATABASE:  DbPutClassAttributeProperty class " << string((*data_in)[0]) << " not found." << endl;
	} else {
		sscanf((*data_in)[index],"%d",&num_attr); index++;
		t_tango_class& classe_trovata = *(*(it));
		t_attribute_property* temp_attribute_property;
		std::vector<t_attribute_property*>::iterator attr_prop_it;

		for(unsigned int j = 0; j < num_attr; j++)
		{
			// search an attribute property for this attribute
			attr_prop_it = find_if(classe_trovata.attribute_properties.begin(),classe_trovata.attribute_properties.end(), hasAttributeName<t_attribute_property>(string((*data_in)[index])));
			if (attr_prop_it != classe_trovata.attribute_properties.end())
			{
				temp_attribute_property = (*attr_prop_it);
			} else {
				// the property is not yet in the file: we add it
				temp_attribute_property = new t_attribute_property;
				temp_attribute_property->attribute_name = string((*data_in)[index]);
				classe_trovata.attribute_properties.push_back(temp_attribute_property); 
				
			}
			index++;
			sscanf((*data_in)[index],"%d",&num_prop); index++; 
			for (unsigned int i = 0; i < num_prop; i++)
			{
				bool exist = false;
				for(unsigned int k = 0; k < temp_attribute_property->properties.size(); k++)
				{
		  		if (equalsIgnoreCase(temp_attribute_property->properties[k]->name, (*data_in)[index].in())) 
		  		{
						index++;
						temp_attribute_property->properties[k]->value.erase(temp_attribute_property->properties[k]->value.begin(),
                                                        temp_attribute_property->properties[k]->value.begin()+temp_attribute_property->properties[k]->value.size());
						sscanf((*data_in)[index],"%d",&num_vals); index++;
						for(unsigned int n = 0; n < num_vals; n++)
						{
							temp_attribute_property->properties[k]->value.push_back(string((*data_in)[index])); index++;
						}

						//(*dev_it)->attribute_properties[j]->properties[k]->value.push_back( string((*data_in)[index]) );index++;
						if (index >= data_in->length())
						{
							write_file();
							return ret;
						}
						exist = true;
					}
				}
				if ( !exist )
				{
					t_property* new_prop = new t_property;
					new_prop->name = (*data_in)[index]; index++;

					sscanf((*data_in)[index],"%d",&num_vals); index++;
					for(unsigned int n = 0; n < num_vals; n++)
					{
						new_prop->value.push_back(string((*data_in)[index])); index++;
					}

					//makeStringArray( string((*data_in)[index]), new_prop->value);index++;
					//new_prop->value.push_back( string((*data_in)[index]) );index++;
					temp_attribute_property->properties.push_back(new_prop);
					if (index >= data_in->length())
					{
							write_file();
							return ret;
					}
				}

			 } 
		}
			
	} 
				

	write_file();
	return ret;
};


CORBA::Any*   FileDatabase :: DbDeleteClassAttributeProperty(CORBA::Any&)
{ 
	CORBA::Any* ret = new CORBA::Any;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbDeleteClassAttributeProperty");
	return ret;
};


CORBA::Any*  FileDatabase :: DbGetDeviceList(CORBA::Any& send)
{ 
	CORBA::Any* any_ptr = new CORBA::Any();	
//	Tango::DevVarStringArray* data_in  = new DevVarStringArray;
	const Tango::DevVarStringArray* data_in;
	Tango::DevVarStringArray* data_out  = new DevVarStringArray;
	
	cout4 << "FILEDATABASE: entering DbGetDeviceList" << endl;

	send >>= data_in;

		
	if (data_in->length() == 2) 
	{
		if ( equalsIgnoreCase((*data_in)[0].in(), m_server.name + "/" + m_server.instance_name))
		{
			unsigned int i;		
			for(i = 0; i < m_server.classes.size(); i++)
			{
				//if ( strcmp((*data_in)[1], m_server.classes[i]->name.c_str()) == 0 )
				if ( equalsIgnoreCase ((*data_in)[1].in(), m_server.classes[i]->name))
				{
					data_out->length(m_server.classes[i]->devices.size());
					for (unsigned int j = 0; j < m_server.classes[i]->devices.size(); j++)
					{
						(*data_out)[j] = CORBA::string_dup( m_server.classes[i]->devices[j]->name.c_str() );
					}
					break;
				}
			}
			
			if (i == m_server.classes.size())
			{
				TangoSys_MemStream desc;
				desc << "File database: Can't find class " << (*data_in)[1];
				desc << " in file " << filename << "." << ends;
				ApiConnExcept::throw_exception((const char *)"API_DatabaseFileError",
				               			desc.str(),
				               			(const char *)"FileDatabase::DbGetDeviceList");
			}
		}
	}

	(*any_ptr) <<= data_out;	
	//  (*any_ptr) <<= (*data_out);
	//CORBA::Any_var* any_var = new CORBA::Any_var;
	//(*any_var) = any_ptr;
	
	//return *any_var;
	return any_ptr;
};


CORBA::Any*  FileDatabase :: DbInfo(CORBA::Any&){ 
	CORBA::Any* any_ptr = new CORBA::Any();
	
	Tango::DevVarStringArray* data_out  = new DevVarStringArray;
	data_out->length(13);
	char temp_str[256];
	sprintf(temp_str,"TANGO FileDatabase  %s", filename.c_str());
	(*data_out)[0] = CORBA::string_dup(temp_str);
	(*data_out)[1] = CORBA::string_dup("");
	(*data_out)[2] = CORBA::string_dup("Running since ----");
	(*data_out)[3] = CORBA::string_dup("");
	sprintf(temp_str,"Devices defined = %d", m_server.devices.size());
	(*data_out)[4] = CORBA::string_dup(temp_str);
	sprintf(temp_str,"Devices exported = %d", m_server.devices.size());
	(*data_out)[5] = CORBA::string_dup(temp_str);
	(*data_out)[6] = CORBA::string_dup("Device servers defined = 1");
	(*data_out)[7] = CORBA::string_dup("Device servers exported = 1");
	(*data_out)[8] = CORBA::string_dup("");
	long temp_long = 0;
	for(std::vector<t_tango_class*>::iterator it = m_server.classes.begin(); it != m_server.classes.end(); it++)
		temp_long += (*it)->properties.size();
	sprintf(temp_str,"Class properties defined = %ld", temp_long);
	(*data_out)[9] = CORBA::string_dup(temp_str);
	
	temp_long = 0;
	for(std::vector<t_device*>::iterator ite = m_server.devices.begin(); ite != m_server.devices.end(); ite++)
		temp_long += (*ite)->properties.size();
	sprintf(temp_str,"Device properties defined = %ld", temp_long);
	(*data_out)[10] = CORBA::string_dup(temp_str);

	temp_long = 0;
	for(std::vector<t_tango_class*>::iterator iter = m_server.classes.begin(); iter != m_server.classes.end(); iter++)
		temp_long += (*iter)->attribute_properties.size();
	sprintf(temp_str,"Class attribute properties defined = %ld", temp_long);
	(*data_out)[11] = CORBA::string_dup(temp_str);
	
	temp_long = 0;
	for(std::vector<t_device*>::iterator itera = m_server.devices.begin(); itera != m_server.devices.end(); itera++)
		temp_long += (*itera)->attribute_properties.size();
	sprintf(temp_str,"Device attribute properties defined = %ld", temp_long);
	(*data_out)[12] = CORBA::string_dup(temp_str);
	(*any_ptr) <<= data_out;
	
	return any_ptr;
};

CORBA::Any*  FileDatabase :: DbImportDevice(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;

	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbImportDevice");

	return ret;
};


CORBA::Any*  FileDatabase :: DbExportDevice(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbExportDevice");

	return ret;
};


CORBA::Any*  FileDatabase :: DbUnExportDevice(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbUnExportDevice");

	return ret;
};


CORBA::Any*  FileDatabase :: DbAddDevice(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbAddDevice");

	return ret;
};


CORBA::Any*  FileDatabase :: DbDeleteDevice(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbDeleteDevice");

	return ret;
};


CORBA::Any*  FileDatabase :: DbAddServer(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbAddServer");

	return ret;
};


CORBA::Any*  FileDatabase :: DbDeleteServer(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbDeleteServer");

	return ret;
};


CORBA::Any*  FileDatabase :: DbExportServer(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbExportServer");

	return ret;
};


CORBA::Any*  FileDatabase :: DbUnExportServer(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbExportDevice");

	return ret;
};


CORBA::Any*  FileDatabase :: DbGetServerInfo(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"Filedatabase::DbGetServerInfo");

	return ret;
};



CORBA::Any*  FileDatabase :: DbGetDeviceMemberList(CORBA::Any&)
{ 
	CORBA::Any* any_ptr = new CORBA::Any();	
	
	Tango::DevVarStringArray* argout = new Tango::DevVarStringArray();
	argout->length(1);
	(*argout)[0] = CORBA::string_dup("NoMember");
	(*any_ptr) <<= argout;
		
	return any_ptr;
};


CORBA::Any*  FileDatabase :: DbGetDeviceExportedList(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbGetDeviceExportedList");

	return ret;
};


CORBA::Any*  FileDatabase :: DbGetDeviceFamilyList(CORBA::Any&)
{ 
	CORBA::Any* any_ptr = new CORBA::Any();	
	
	Tango::DevVarStringArray* argout = new Tango::DevVarStringArray();
	argout->length(1);
	(*argout)[0] = CORBA::string_dup("NoDevice");
	(*any_ptr) <<= argout;
	
	return any_ptr;
};

CORBA::Any*  FileDatabase :: DbGetDeviceDomainList(CORBA::Any&)
{ 
	CORBA::Any* any_ptr = new CORBA::Any();	
	
	Tango::DevVarStringArray* argout = new Tango::DevVarStringArray();
	argout->length(1);
	(*argout)[0] = CORBA::string_dup("NoDevice");
	(*any_ptr) <<= argout;
	
	return any_ptr;
};

/** At the moment we have no information about general properties
 * so I put nothing out
 */
CORBA::Any*  FileDatabase :: DbGetProperty(CORBA::Any& send)
{ 
	CORBA::Any* any_ptr = new CORBA::Any();	
	
	
	const Tango::DevVarStringArray* data_in;
	Tango::DevVarStringArray* data_out  = new DevVarStringArray;
	char num_attr_str[256];
	const char* zero_str = "0";
	
	cout4 << "FILEDATABASE: entering DbGetProperty" << endl;

	send >>= data_in;
	
	data_out->length(2);
	sprintf(num_attr_str,"%lu",data_in->length()-1);
	(*data_out)[0] = CORBA::string_dup((*data_in)[0]);
	(*data_out)[1] = CORBA::string_dup(zero_str);
	
	(*any_ptr) <<= data_out;
	
	return any_ptr;
};


CORBA::Any*  FileDatabase :: DbPutProperty(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbPutProperty");
	return ret;
};


CORBA::Any*  FileDatabase :: DbDeleteProperty(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbDeleteProperty");
	return ret;
};


CORBA::Any*  FileDatabase :: DbGetAliasDevice(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbGetAliasDevice");
	return ret;
};


CORBA::Any*  FileDatabase :: DbGetDeviceAlias(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbGetDeviceAlias");
	return ret;
};


CORBA::Any*  FileDatabase :: DbGetAttributeAlias(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;

	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbGetAttributeAlias");
	return ret;
};


CORBA::Any*  FileDatabase :: DbGetDeviceAliasList(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbGetDeviceAliasList");
	return ret;
};


CORBA::Any*  FileDatabase :: DbGetAttributeAliasList(CORBA::Any&)
{ 
	CORBA::Any* ret = NULL;
	
	Tango::Except::throw_exception((const char *)"Method not implemented.",
	   				                     (const char *)"Call to a Filedatabase not implemented.",
					                       (const char *)"DbGetAttributeAliasList");
	return ret;
};



} // end of namespace Tango
