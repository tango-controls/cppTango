/*
 * Check size of user classes/structure (For compatibility prurpose)
 *
 * This test suite has to be run on a redhate4 host
 * Otherwise, you have to change the classes/structures size
 *
 */

#include <tango.h>

void check_size(const char *,long,long, bool*);

#define	coutv	if (verbose == true) cout

bool verbose = false;

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{

	if (argc == 2)
	{
		if (strcmp(argv[1],"-v") == 0)
			verbose = true;
	}

	bool isOK = true;

	check_size("Attr",sizeof(Attr),128, &isOK);  // Was 52 in V4
	check_size("SpectrumAttr",sizeof(SpectrumAttr),144, &isOK);	// Was 60 in V4
	check_size("ImageAttr",sizeof(ImageAttr),160, &isOK);	// Was 68 in V4

	check_size("DeviceImpl",sizeof(DeviceImpl),1760, &isOK);
	check_size("Device_2Impl",sizeof(Device_2Impl),1776, &isOK);

	check_size("DeviceClass",sizeof(DeviceClass),416, &isOK);  	// Was 48 in V4
	check_size("Util",sizeof(Util),1616, &isOK);	// Was 68 in V4

	check_size("Attribute", sizeof(Attribute), 16, &isOK);
	check_size("WAttribute", sizeof(WAttribute), 24, &isOK);
	check_size("MultiAttribute",sizeof(MultiAttribute),80, &isOK);

	check_size("Command",sizeof(Command),72, &isOK);
	check_size("TemplCommand",sizeof(TemplCommand),112, &isOK);
	check_size("TemplCommandIn",sizeof(TemplCommandIn<int>),136, &isOK);
	check_size("TemplCommandOut",sizeof(TemplCommandOut<void>),136, &isOK);
	check_size("TemplCommandInOut",sizeof(TemplCommandInOut<int,int>),136, &isOK);

	check_size("DeviceProxy",sizeof(DeviceProxy),576, &isOK);	// Was 100 in V4
	check_size("DeviceData",sizeof(DeviceData),32, &isOK);
	check_size("DeviceDataHistory",sizeof(DeviceDataHistory),80, &isOK);
	check_size("DeviceAttribute",sizeof(DeviceAttribute),200, &isOK);	// Was 52 in V4
	check_size("DeviceAttributeHistory",sizeof(DeviceAttributeHistory),232, &isOK);   // Was 76 in V4

	check_size("ApiUtil",sizeof(ApiUtil),312, &isOK);
	check_size("Database",sizeof(Database),624, &isOK);    // Was 108 in V4
	check_size("DbDevice",sizeof(DbDevice),32, &isOK);
	check_size("DbClass",sizeof(DbClass),32, &isOK);
	check_size("DbServer",sizeof(DbServer),32, &isOK);
	check_size("DbDatum",sizeof(DbDatum),56, &isOK);
	check_size("DbData",sizeof(DbData),24, &isOK);

	check_size("CmdDoneEvent",sizeof(CmdDoneEvent),48, &isOK);
	check_size("AttrReadEvent",sizeof(AttrReadEvent),48, &isOK);
	check_size("AttrWrittenEvent",sizeof(AttrWrittenEvent),40, &isOK);
	check_size("CallBack",sizeof(CallBack),16, &isOK);

	check_size("EncodedAttribute",sizeof(EncodedAttribute),56, &isOK);

	check_size("Group",sizeof(Group),152, &isOK);

	check_size("MultiAttrProp<T>",sizeof(MultiAttrProp<DevShort>),592, &isOK);
	check_size("AttrProp<T>",sizeof(AttrProp<DevShort>),32, &isOK);
	check_size("DoubleAttrProp<T>",sizeof(DoubleAttrProp<DevShort>),48, &isOK);

	check_size("DevicePipe",sizeof(DevicePipe),112, &isOK);
	check_size("DevicePipeBlob",sizeof(DevicePipeBlob),80, &isOK);

	assert(isOK);
}

void check_size(const char *class_name,long class_size,long std_size, bool* isOK)
{
	coutv << "Size of " << class_name << " = " << class_size << endl;
	if (std_size == class_size)
		cout << "   Size of " << class_name << " --> OK" << endl;
	else {
		cout << "   Size of " << class_name << " should be " << std_size << " and is " << class_size
			 << " --> NOK !!!!!!!!!!!" << endl;
		*(isOK) = false;
	}
}


