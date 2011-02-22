static const char *RcsId = "$Header$";

//+============================================================================
//
// file :               BlackBox.cpp
//
// description :        C++ source code for the BlackBoxElt and BlackBox
//			classes. These classes are used to implement the 
//			tango device server black box. There is one
//			black box for each Tango device. This black box
//			keeps info. on all the activities on a device.
//			A client is able to retrieve these data via a Device
//			attribute
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:39  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:13  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:27  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <blackbox.h>

#include <stdio.h>

#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif /* WIN32 */

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		BlackBoxElt::BlackBoxElt 
// 
// description : 	Constructor for the BlackBoxElt class.
//			This constructor simply set the internal value to their
//			default
//
//--------------------------------------------------------------------------

BlackBoxElt::BlackBoxElt()
{
	req_type = Req_Unknown;
	attr_type = Attr_Unknown;
	op_type = Op_Unknown;
	when.tv_sec = when.tv_usec = 0;
	host = "Unknown";	
	host_ip = NULL;
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::BlackBox 
// 
// description : 	Two constructors for the BlackBox class. The first one
//			does not take any argument and construct a black box
//			with the default depth.
//			The second one create a black box with a depth defined 
//			by the argument.
//
// argument : in : 	- max_size : The black box depth
//
//--------------------------------------------------------------------------

BlackBox::BlackBox():box(DefaultBlackBoxDepth)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = DefaultBlackBoxDepth;	
	
	Tango::Util *tg = Tango::Util::instance();
	orb = tg->get_orb();	
}

BlackBox::BlackBox(long max_size):box(max_size)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = max_size;		

	Tango::Util *tg = Tango::Util::instance();
	orb = tg->get_orb();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_attr
// 
// description : 	This method insert a new element in the black box when
//			this element is a attribute
//
// argument : in : 	- attr : The attribute type
//
//--------------------------------------------------------------------------


void BlackBox::insert_attr(BlackBoxElt_AttrType attr)
{
	
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Attribute;
	box[insert_elt].attr_type = attr;
	box[insert_elt].op_type = Op_Unknown;

#ifdef WIN32
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
			
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_attr
// 
// description : 	This method insert a new element in the black box when
//			this element is a attribute
//
// argument : in : 	- attr : The attribute type
//			- host : The client host
//
//--------------------------------------------------------------------------


void BlackBox::insert_attr(BlackBoxElt_AttrType attr,const char *host)
{

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Attribute;
	box[insert_elt].attr_type = attr;
	box[insert_elt].op_type = Op_Unknown;
	box[insert_elt].host = host;

#ifdef WIN32
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif
			
//
// manage insert and read indexes
//

	inc_indexes();
}


//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_cmd
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to the operation command_inout
//
// argument : in : 	- c_name : The command name
//
//--------------------------------------------------------------------------


void BlackBox::insert_cmd(const char *cmd)
{

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = Op_Command_inout;
	box[insert_elt].cmd_name = cmd;
#ifdef WIN32
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
}


//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_cmd
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to the operation command_inout
//
// argument : in : 	- c_name : The command name
//			- host : The client host
//
//--------------------------------------------------------------------------


void BlackBox::insert_cmd(const char *cmd,const char *host)
{

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = Op_Command_inout;
	box[insert_elt].cmd_name = cmd;
	box[insert_elt].host = host;
#ifdef WIN32
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif
	
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_op
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to an operation which is not
//			the command_inout operation
//
// argument : in : 	- cmd : The operation type
//
//--------------------------------------------------------------------------


void BlackBox::insert_op(BlackBoxElt_OpType op)
{

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = op;
#ifdef WIN32
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_op
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to an operation which is not
//			the command_inout operation
//
// argument : in : 	- cmd : The operation type
//			- host : The client host
//
//--------------------------------------------------------------------------


void BlackBox::insert_op(BlackBoxElt_OpType op,const char *host)
{

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = op;
	box[insert_elt].host = host;
#ifdef WIN32
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif
	
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::inc_indexes
// 
// description : 	This private method increment the indexes used to acces
//			the box itself. This is necessary because the box must
//			be managed as a circular buffer
//
//--------------------------------------------------------------------------


void BlackBox::inc_indexes()
{
	insert_elt++;
	if (insert_elt == max_elt)
		insert_elt = 0;
		
	if (nb_elt != max_elt)
		nb_elt++;
}

//+-------------------------------------------------------------------------
//
// method : 		get_client_host
// 
// description : 	This private method retrieves the client host IP
//			address (the number). IT USES ORBACUS SPECIFIC
//			CLASSES
//
//--------------------------------------------------------------------------

void BlackBox::get_client_host()
{
	OCI::Current_var current;
	try
	{
		CORBA::Object_var baseCurrent = orb->resolve_initial_references("OCICurrent");
		current = OCI::Current::_narrow(baseCurrent);
	}
	catch(...)
	{
		current = NULL;
	}
	
	if (current != (OCI::Current_var)NULL)
	{
		
		OCI::TransportInfo_var info = current->get_oci_transport_info();
		OCI::IIOP::TransportInfo_var iiopInfo = OCI::IIOP::TransportInfo::_narrow(info);
		
		if (!CORBA::is_nil(iiopInfo))
		{
			if (box[insert_elt].host_ip != NULL)
				OCI::IIOP::InetAddr_free(box[insert_elt].host_ip);

			box[insert_elt].host_ip = iiopInfo->remote_addr();
			box[insert_elt].host.erase();
			box[insert_elt].host = "Defined";
		}
		else
		{
			box[insert_elt].host.erase();
			box[insert_elt].host = "Unknown";
		}
	}
}
	
//+-------------------------------------------------------------------------
//
// method : 		BlackBox::build_info_as_str
// 
// description : 	Translate all the info stored in a black box element
//			into a readable string.
//
// argument : in : 	- index : The black box element index
//
//--------------------------------------------------------------------------

void BlackBox::build_info_as_str(long index)
{
	char date_str[25];
//
// Convert time to a string
//

	date_ux_to_str(box[index].when,date_str);
	elt_str = date_str; 
	
//
// Add request type and command name in case of
//

	elt_str = elt_str + " : ";
	if (box[index].req_type == Req_Operation)
	{
		elt_str = elt_str + "Operation ";
		switch (box[index].op_type)
		{
		case Op_Command_inout :
			elt_str = elt_str + "command_inout (cmd = " + box[index].cmd_name + ") ";
			break;
			
		case Op_Ping :
			elt_str = elt_str + "ping ";
			break;
			
		case Op_Info :
			elt_str = elt_str + "info ";
			break;

		case Op_BlackBox :
			elt_str = elt_str + "blackbox ";
			break;

		case Op_Command_list :
			elt_str = elt_str + "command_list_query ";
			break;
		
		case Op_Command :
			elt_str = elt_str + "command_query ";
			break;
										
		case Op_Get_Attr_Config :
			elt_str = elt_str + "get_attribute_config ";
			break;
			
		case Op_Set_Attr_Config :
			elt_str = elt_str + "set_attribute_config ";
			break;
			
		case Op_Read_Attr :
			elt_str = elt_str + "read_attributes ";
			break;
			
		case Op_Write_Attr :
			elt_str = elt_str + "write_attributes ";
			break;

		case Op_Unknown :
			elt_str = elt_str + "unknown operation !!!!!";
			return;
		}
	}	
	else if (box[index].req_type == Req_Attribute)
	{
		elt_str = elt_str + "Attribute ";
		switch (box[index].attr_type)
		{
		case Attr_Name :
			elt_str = elt_str + "name ";
			break;
			
		case Attr_Description :
			elt_str = elt_str + "description ";
			break;

		case Attr_Status :
			elt_str = elt_str + "status ";
			break;
			
		case Attr_State :
			elt_str = elt_str + "state ";
			break;
						
		case Attr_Unknown :
			elt_str = elt_str + "unknown attribute !!!!!";
			return;
		}
	}
	else
	{
		elt_str = elt_str + "Unknown CORBA request type !!!!!";
		return;
	}

//
// Add client host if defined
//

	if (box[index].host != "Unknown")
	{	
        	struct hostent *ho;
#ifdef WIN32
		struct in_addr ad;
		ad.s_addr = (box[index].host_ip[0] << 24) + \
			    (box[index].host_ip[1] << 16) + \
			    (box[index].host_ip[2] << 8) + \
			     box[index].host_ip[3];
		unsigned long ip_net = htonl(ad.s_addr);
		ho = gethostbyaddr((char *)(&ip_net),sizeof(ip_net),AF_INET);
#else
#ifdef __hpux
		struct in_addr ad;
		
		ad.s_addr = (box[index].host_ip[0] << 24) + \
			    (box[index].host_ip[1] << 16) + \
			    (box[index].host_ip[2] << 8) + \
			     box[index].host_ip[3];
		ho = gethostbyaddr((char *)(&ad),sizeof(struct in_addr),AF_INET);
#else
		unsigned long ip = (box[index].host_ip[0] << 24) + \
				   (box[index].host_ip[1] << 16) + \
				   (box[index].host_ip[2] << 8) + \
				    box[index].host_ip[3];
#ifdef __linux
		unsigned long ip_net = htonl(ip);
		ho = gethostbyaddr((char *)(&ip_net),sizeof(ip),AF_INET);
#else
		ho = gethostbyaddr((char *)(&ip),sizeof(ip),AF_INET);
#endif /* __linux */
#endif /* __hpux */
#endif /* WIN32 */
		if (ho == NULL)
		{
			return;
		}
		else
		{
			elt_str = elt_str + "requested from ";
			elt_str = elt_str + ho->h_name;
		}
	}	
		
	return;
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::read
// 
// description : 	Read black box element as strings. The newest element
//			is return in the first position
//
// argument : in : 	- index : The number of element to read
//
//--------------------------------------------------------------------------

Tango::DevVarStringArray *BlackBox::read(long wanted_elt)
{

//
// Throw exeception if the wanted element is stupid and if there is no element
// stored in the black box
//

	if (wanted_elt <= 0)
	{
		Except::throw_exception((const char *)"API_BlackBoxArgument",
				      (const char *)"Argument to read black box out of range",
				      (const char *)"BlackBox::read");
	}
	if (nb_elt == 0)
	{
		Except::throw_exception((const char *)"API_BlackBoxEmpty",
				      (const char *)"Nothing stored yet in black-box",
				      (const char *)"BlackBox::read");
	}
			
//
// Limit wanted element to a reasonable value
//

	if (wanted_elt > max_elt)
		wanted_elt = max_elt;
		
	if (wanted_elt > nb_elt)
		wanted_elt = nb_elt;
		
//
// Read black box elements
//

	Tango::DevVarStringArray *ret;
	try
	{
	
		ret = new Tango::DevVarStringArray(wanted_elt);
                ret->length(wanted_elt);

		long read_index;
		if (insert_elt == 0)
			read_index = max_elt - 1;
		else
			read_index = insert_elt - 1;
		for (long i = 0;i < wanted_elt;i++)
		{
			build_info_as_str(read_index);
			(*ret)[i] = elt_str.c_str();
		
			read_index--;
			if (read_index < 0)
				read_index = max_elt - 1;
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"BlackBox::read");
	}		
	return(ret);
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::date_ux_to_str
// 
// description : 	Convert a UNIX date (number of seconds since EPOCH)
//			to a string of the following format :
//			dd/mm/yyyy hh24:mi:ss:xx
//
// argument : in : 	- ux_date : The UNIX date in a timeval structure
//			- str_date : Pointer to char array where the date will
//				     be stored (must be allocated)
//
//--------------------------------------------------------------------------

void BlackBox::date_ux_to_str(timeval &ux_date,char *str_date)
{
	long i;
	char month[5];
	char *ux_str;
	char unix_date[30];

/* Convert UNIX date to a string in UNIX format */

	ux_str = ctime((time_t *)&(ux_date.tv_sec));
	strcpy(unix_date,ux_str);

/* Copy day */

	for (i = 0;i < 2;i++)
		str_date[i] = unix_date[i + 8];
	str_date[2] = '/';
	str_date[3] = '\0';
	if (str_date[0] == ' ')
		str_date[0] = '0';

/* Copy month */

	for (i = 0;i < 3;i++)
		month[i] = unix_date[i + 4];
	month[3] = '\0';

	switch(month[0])
	{
		case 'J' : if (month[1] == 'u')
			   {
				if (month[2] == 'n')
					strcat(str_date,"06/");
				else
					strcat(str_date,"07/");
			   }
			   else
				strcat(str_date,"01/");
			   break;

		case 'F' : strcat(str_date,"02/");
			   break;

		case 'M' : if (month[2] == 'r')
				strcat(str_date,"03/");
			   else
				strcat(str_date,"05/");
			   break;

		case 'A' : if (month[1] == 'p')
				strcat(str_date,"04/");
			   else
				strcat(str_date,"08/");
			   break;

		case 'S' : strcat(str_date,"09/");
			   break;

		case 'O' : strcat(str_date,"10/");
		           break;

		case 'N' : strcat(str_date,"11/");
			   break;

		case 'D' : strcat(str_date,"12/");
			   break;
	}

	str_date[6] = '\0';

/* Copy year */

	strcat(str_date,&(unix_date[20]));
	str_date[10] = '\0';
	
/* Copy date remaining */

	strcat(str_date," ");
	for (i = 0;i < 8;i++)
		str_date[i + 11] = unix_date[i + 11];
	str_date[19] = '\0';
	
/* Add milliseconds */

#ifdef WIN32
	sprintf(&(str_date[19]),":%.2d",(int)(ux_date.tv_usec/10));
#else
	sprintf(&(str_date[19]),":%.2d",(int)(ux_date.tv_usec/10000));
#endif

}

} // End of Tango namespace
