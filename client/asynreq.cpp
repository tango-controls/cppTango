static const char *RcsId = "$Id$\n$Name$";

//
// cpp 	- C++ source code file for TANGO AsynReq class
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 	- January 2003
//
// $Log$
// Revision 3.1.2.1  2004/03/02 07:40:23  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.1  2003/05/28 14:42:55  taurel
// Add (conditionaly) autoconf generated include file
//
// Revision 3.0  2003/03/25 16:30:46  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 1.1  2003/03/20 08:56:12  taurel
// New file to support asynchronous calls
//
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

                                                    
namespace Tango
{

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::store_request()
// 
// description : 	Store a new request in the polling request map
//
// argin(s) :		req : The CORBA request object
//			type : The request type
//
// return :		The asynchronous request identifier
//
//-----------------------------------------------------------------------------

long AsynReq::store_request(CORBA::Request_ptr req,TgRequest::ReqType type)
{
	
//
// First, get a request identifier
//

	long req_id = ui_ptr->get_ident();
	
//
// Store couple ident/request in map
//

	TgRequest tmp_req(req,type);
	
	omni_mutex_lock sync(*this);
	asyn_poll_req_table.insert(map<long,TgRequest>::value_type(req_id,tmp_req));
	
	return req_id;
}

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::store_request()
// 
// description : 	Store a new request in the callback request maps
//
// argin(s) :		req : The CORBA request object
//			cb : The callback object
//			dev : The device
//			type : The request type
//
//-----------------------------------------------------------------------------


void AsynReq::store_request(CORBA::Request_ptr req,
		   	    CallBack *cb,
		   	    Connection *dev,
		   	    TgRequest::ReqType type)
{
//
// Store couple device_ptr/request in map
//

	TgRequest tmp_req_dev(req,type,cb);
	TgRequest tmp_req(dev,type,cb);
	
	omni_mutex_lock sync(*this);
	cb_dev_table.insert(map<Connection *,TgRequest>::value_type(dev,tmp_req_dev));
	cb_req_table.insert(map<CORBA::Request_ptr,TgRequest>::value_type(req,tmp_req));
	
}

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::get_request()
// 
// description : 	Return the Tango request object associated to the id
//			passed as method argument. This method is used
//			for asynchronous polling mode
//
// argin(s) :		req_id : The Tango request identifier
//
// return :		The Tango request object
//
//-----------------------------------------------------------------------------

Tango::TgRequest &AsynReq::get_request(long req_id)
{
	map<long,TgRequest>::iterator pos;

	omni_mutex_lock sync(*this);	
	pos = asyn_poll_req_table.find(req_id);
	
	if (pos == asyn_poll_req_table.end())
	{
		TangoSys_OMemStream desc;
		desc << "Failed to find a asynchronous polling request ";
		desc << "with id = " << req_id << ends;
                ApiAsynExcept::throw_exception((const char*)"API_BadAsynPollId",
                        		       desc.str(),
					       (const char*)"AsynReq::get_request()");
	}
		
	return pos->second;
}

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::get_request()
// 
// description : 	Return the Tango request object associated to the CORBA
//			request object passed as method argument. 
//			This method is used for asynchronous callback mode
//
// argin(s) :		req : The CORBA request object
//
// return :		The Tango request object
//
//-----------------------------------------------------------------------------

Tango::TgRequest &AsynReq::get_request(CORBA::Request_ptr req)
{
	map<CORBA::Request_ptr,TgRequest>::iterator pos;

	omni_mutex_lock sync(*this);
	pos = cb_req_table.find(req);

	if (pos == cb_req_table.end())
	{
		TangoSys_OMemStream desc;
		desc << "Failed to find a asynchronous callback request ";
                ApiAsynExcept::throw_exception((const char*)"API_BadAsyn",
                        		       desc.str(),
					       (const char*)"AsynReq::get_request() (by request)");
	}
		
	return pos->second;
}

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::get_request()
// 
// description : 	Return the Tango request object associated to the device
//			passed as method argument and for which replies are
//			already arrived. 
//			This method is used for asynchronous callback mode
//
// argin(s) :		dev : The Tango device
//
// return :		The Tango request object
//
//-----------------------------------------------------------------------------

Tango::TgRequest *AsynReq::get_request(Tango::Connection *dev)
{
	multimap<Connection *,TgRequest>::iterator pos;

	bool found = false;
	omni_mutex_lock sync(*this);
	for (pos = cb_dev_table.lower_bound(dev);pos != cb_dev_table.upper_bound(dev);++pos)
	{
		if (pos->second.arrived == true)
		{
			found = true;
			break;
		}
	}	

	if (found == false)
		return NULL;
	else
		return &(pos->second);
}

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::mark_as_arrived()
// 
// description : 	Mark a request as arrived in the callback device map
//
// argin(s) :		req : The CORBA request object
//
//-----------------------------------------------------------------------------

void AsynReq::mark_as_arrived(CORBA::Request_ptr req)
{
	multimap<Connection *,TgRequest>::iterator pos;

	for (pos = cb_dev_table.begin();pos != cb_dev_table.end();++pos)
	{
		if (pos->second.request == req)
		{
			pos->second.arrived = true;
			break;
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::remove_request()
// 
// description : 	Remove a request from the object map. The Id is passed
//			as input argument. This method is used for the polling
//			mode
//
// argin(s) :		req_id : The Tango request identifier
//
//-----------------------------------------------------------------------------

void AsynReq::remove_request(long req_id)
{
	map<long,TgRequest>::iterator pos;

	omni_mutex_lock sync(*this);	
	pos = asyn_poll_req_table.find(req_id);
	
	if (pos == asyn_poll_req_table.end())
	{
		TangoSys_OMemStream desc;
		desc << "Failed to find a asynchronous polling request ";
		desc << "with id = " << req_id << ends;
                ApiAsynExcept::throw_exception((const char*)"API_BadAsynPollId",
                        		       desc.str(),
					       (const char*)"AsynReq::remove_request()");
	}
	else
	{
		CORBA::release(pos->second.request);
		asyn_poll_req_table.erase(pos);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		AsynReq::remove_request()
// 
// description : 	Remove a request from the two map used for asynchronous
//			callback request. This method is used for the callback
//			mode
//
// argin(s) :		dev : The Tango device
//			req : The CORBA request object
//
//-----------------------------------------------------------------------------

void AsynReq::remove_request(Connection *dev,CORBA::Request_ptr req)
{
	multimap<Connection *,TgRequest>::iterator pos;
	map<CORBA::Request_ptr,TgRequest>::iterator pos_req;

	omni_mutex_lock sync(*this);
	for (pos = cb_dev_table.lower_bound(dev);pos != cb_dev_table.upper_bound(dev);++pos)
	{
		if (pos->second.request == req)
		{
			CORBA::release(pos->second.request);
			cb_dev_table.erase(pos);
			break;
		}
	}

	pos_req = cb_req_table.find(req);
	if (pos_req != cb_req_table.end())
	{
		cb_req_table.erase(pos_req);
	}
		
}


} // End of tango namespace
