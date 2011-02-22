/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	AttributeProxy *attr;
	
	if (argc != 2)
	{
		cout << "usage: %s attribute" << endl;
		exit(-1);
	}

	string attr_name = argv[1];

	try 
	{
		attr = new AttributeProxy(attr_name);
		cout << "new AttributeProxy() returned" << endl;
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }

	attr->get_device_proxy()->set_transparency_reconnection(true);
	
	while (1)
	{
		cout << "Hit any key :" << endl;
		char bid;
		cin >> bid;

		try
		{	
			attr->ping();
			cout << "Ping successfull" << endl;
		}
		catch (Tango::CommunicationFailed &e)
		{
			cerr << "Commnunication Failed exception" << endl;
			Except::print_exception(e);
		}
		catch (Tango::ConnectionFailed &e)
		{
			cerr << "Connection Failed exception" << endl;
			Except::print_exception(e);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
		}
		catch (...)
		{
			cout << "Unknown exception" << endl;
		}
		
	}
		
	return 0;

}
