static const char *RcsId = "$Id$\n$Name$";
//
// dbapi_history.cpp - C++ source code file for TANGO dbapi class DbHistory
//
// programmer 	- JL Pons (pons@esrf.fr)
//
// original 	- Feb 2007
//
// $Log$
//
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
                                                      
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DbHistory::DbHistory() - Constructs a property
//
//-----------------------------------------------------------------------------

DbHistory::DbHistory(string _propname,string _date,vector<string> &svalues) {

  propname = _propname;
  date = format_mysql_date(_date);
  deleted = (svalues.size()==0);
  make_db_datum(svalues);

}

//-----------------------------------------------------------------------------
//
// DbHistory::DbHistory() - Constructs an attribute property.
//
//-----------------------------------------------------------------------------

DbHistory::DbHistory(string _propname,string _attname,string _date,vector<string> &svalues) {

  propname = _propname;
  attname = _attname;
  date = format_mysql_date(_date);
  deleted = (svalues.size()==0);
  make_db_datum(svalues);

}

//-----------------------------------------------------------------------------
//
// DbHistory::get_name() - Returns property name.
//
//-----------------------------------------------------------------------------

string DbHistory::get_name() {

  return propname;
 
}

//-----------------------------------------------------------------------------
//
// DbHistory::get_attribute_name() - Returns attribute name.
//                                   Used when retrieving attribute property.
//
//-----------------------------------------------------------------------------

string DbHistory::get_attribute_name() {

  return attname;

}

//-----------------------------------------------------------------------------
//
// DbHistory::get_date() - Returns the update date.
//
//-----------------------------------------------------------------------------

string DbHistory::get_date() {

  return date;

}

//-----------------------------------------------------------------------------
//
// DbHistory::get_value() - Returns the value.
//
//-----------------------------------------------------------------------------

DbDatum DbHistory::get_value() {

  return value;

}

//-----------------------------------------------------------------------------
//
// DbHistory::is_deleted() - Return true if the property is deleted.
//
//-----------------------------------------------------------------------------

bool DbHistory::is_deleted() {

  return deleted;

}

//-----------------------------------------------------------------------------
//
// DbHistory::format_mysql_date() - Format mysql date using format
//                                  DD/MM/YYYY hh:mm:ss
//
//-----------------------------------------------------------------------------
  
string DbHistory::format_mysql_date(string _date) {

    // Handle MySQL date formating
    if( _date.find("-")!=string::npos )
      return _date.substr(8,2) + "/" + _date.substr(5,2) + "/" + _date.substr(0,4) + " " +
             _date.substr(11,2) + ":" + _date.substr(14,2) + ":" + _date.substr(17,2);
    else
      return _date.substr(6,2) + "/" + _date.substr(4,2) + "/" + _date.substr(0,4) + " " +
             _date.substr(8,2) + ":" + _date.substr(10,2) + ":" + _date.substr(12,2);

}

//-----------------------------------------------------------------------------
//
// DbHistory::make_db_datum() - Build the value as DbDatum
//
//-----------------------------------------------------------------------------

void DbHistory::make_db_datum(vector<string> &values) {

  value.name = propname;
  value.value_string.resize(values.size());
  for (int i=0; i<values.size(); i++)
    value.value_string[i] = values[i]; 

}

}
