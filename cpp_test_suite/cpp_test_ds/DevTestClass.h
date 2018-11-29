#ifndef _DEV_TESTCLASS_H
#define _DEV_TESTCLASS_H

#include <tango.h>
#include <DevTest.h>

//
// Attributes related classes
//

class Short_attrAttr: public Tango::Attr
{
public:
	Short_attrAttr():Attr("Short_attr", Tango::DEV_SHORT, Tango::READ) {};
	~Short_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Short_attr(att);}
};

class Long_attrAttr: public Tango::Attr
{
public:
	Long_attrAttr():Attr("Long_attr", Tango::DEV_LONG, Tango::READ) {};
	~Long_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long_attr(att);}
};

class Long64_attrAttr: public Tango::Attr
{
public:
	Long64_attrAttr():Attr("Long64_attr", Tango::DEV_LONG64, Tango::READ) {};
	~Long64_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long64_attr(att);}
};

class Double_attrAttr: public Tango::Attr
{
public:
	Double_attrAttr():Attr("Double_attr", Tango::DEV_DOUBLE, Tango::READ) {};
	~Double_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Double_attr(att);}
};

class String_attrAttr: public Tango::Attr
{
public:
	String_attrAttr():Attr("String_attr", Tango::DEV_STRING, Tango::READ) {};
	~String_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_String_attr(att);}
};

class Float_attrAttr: public Tango::Attr
{
public:
	Float_attrAttr():Attr("Float_attr", Tango::DEV_FLOAT, Tango::READ) {};
	~Float_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Float_attr(att);}
};

class Boolean_attrAttr: public Tango::Attr
{
public:
	Boolean_attrAttr():Attr("Boolean_attr", Tango::DEV_BOOLEAN, Tango::READ) {};
	~Boolean_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Boolean_attr(att);}
};

class UShort_attrAttr: public Tango::Attr
{
public:
	UShort_attrAttr():Attr("UShort_attr", Tango::DEV_USHORT, Tango::READ) {};
	~UShort_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_UShort_attr(att);}
};

class UChar_attrAttr: public Tango::Attr
{
public:
	UChar_attrAttr():Attr("UChar_attr", Tango::DEV_UCHAR, Tango::READ) {};
	~UChar_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_UChar_attr(att);}
};

class ULong_attrAttr: public Tango::Attr
{
public:
	ULong_attrAttr():Attr("ULong_attr", Tango::DEV_ULONG, Tango::READ) {};
	~ULong_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_ULong_attr(att);}
};

class ULong64_attrAttr: public Tango::Attr
{
public:
	ULong64_attrAttr():Attr("ULong64_attr", Tango::DEV_ULONG64, Tango::READ) {};
	~ULong64_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_ULong64_attr(att);}
};

//-----------------------------------------------------------------------------------

class Short_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	Short_spec_attrAttr():SpectrumAttr("Short_spec_attr", Tango::DEV_SHORT, 4) {};
	~Short_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Short_spec_attr(att);}
};

class Long_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	Long_spec_attrAttr():SpectrumAttr("Long_spec_attr", Tango::DEV_LONG, 10) {};
	~Long_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long_spec_attr(att);}
};

class Double_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	Double_spec_attrAttr():SpectrumAttr("Double_spec_attr", Tango::DEV_DOUBLE, 3) {};
	~Double_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Double_spec_attr(att);}
};

class String_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	String_spec_attrAttr():SpectrumAttr("String_spec_attr", Tango::DEV_STRING, 2) {};
	~String_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_String_spec_attr(att);}
};


class Float_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	Float_spec_attrAttr():SpectrumAttr("Float_spec_attr", Tango::DEV_FLOAT, 3) {};
	~Float_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Float_spec_attr(att);}
};

class Boolean_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	Boolean_spec_attrAttr():SpectrumAttr("Boolean_spec_attr", Tango::DEV_BOOLEAN, 5) {};
	~Boolean_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Boolean_spec_attr(att);}
};

class UShort_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	UShort_spec_attrAttr():SpectrumAttr("UShort_spec_attr", Tango::DEV_USHORT, 2) {};
	~UShort_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_UShort_spec_attr(att);}
};

class UChar_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	UChar_spec_attrAttr():SpectrumAttr("UChar_spec_attr", Tango::DEV_UCHAR, 6) {};
	~UChar_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_UChar_spec_attr(att);}
};


//  -----------------------------------------------------------------------------------

class Short_ima_attrAttr: public Tango::ImageAttr
{
public:
	Short_ima_attrAttr():ImageAttr("Short_ima_attr", Tango::DEV_SHORT, 4,4) {};
	~Short_ima_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Short_ima_attr(att);}
};

class Long_ima_attrAttr: public Tango::ImageAttr
{
public:
	Long_ima_attrAttr():ImageAttr("Long_ima_attr", Tango::DEV_LONG, 10,2) {};
	~Long_ima_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long_ima_attr(att);}
};

class Double_ima_attrAttr: public Tango::ImageAttr
{
public:
	Double_ima_attrAttr():ImageAttr("Double_ima_attr", Tango::DEV_DOUBLE, 3,3) {};
	~Double_ima_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Double_ima_attr(att);}
};

class String_ima_attrAttr: public Tango::ImageAttr
{
public:
	String_ima_attrAttr():ImageAttr("String_ima_attr", Tango::DEV_STRING, 2,2) {};
	~String_ima_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_String_ima_attr(att);}
};

//  -----------------------------------------------------------------------------------

class attr_no_dataAttr: public Tango::Attr
{
public:
	attr_no_dataAttr():Attr("attr_no_data", Tango::DEV_SHORT,Tango::READ) {};
	~attr_no_dataAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_no_data(att);}
};

class attr_wrong_typeAttr: public Tango::Attr
{
public:
	attr_wrong_typeAttr():Attr("attr_wrong_type", Tango::DEV_SHORT,Tango::READ) {};
	~attr_wrong_typeAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_wrong_type(att);}
};

class attr_wrong_sizeAttr: public Tango::Attr
{
public:
	attr_wrong_sizeAttr():Attr("attr_wrong_size", Tango::DEV_LONG,Tango::READ) {};
	~attr_wrong_sizeAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_wrong_size(att);}
};

class attr_no_alarmAttr: public Tango::Attr
{
public:
	attr_no_alarmAttr():Attr("attr_no_alarm", Tango::DEV_LONG,Tango::READ) {};
	~attr_no_alarmAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_no_alarm(att);}
};

//  ----------------------------------------------------------------------------------

class Short_attr_with_wAttr: public Tango::Attr
{
public:
	Short_attr_with_wAttr():Attr("Short_attr_with_w", Tango::DEV_SHORT, Tango::READ_WITH_WRITE,"Short_attr_w") {};
	~Short_attr_with_wAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Short_attr_with_w(att);}
};

class Long_attr_with_wAttr: public Tango::Attr
{
public:
	Long_attr_with_wAttr():Attr("Long_attr_with_w", Tango::DEV_LONG, Tango::READ_WITH_WRITE,"Long_attr_w") {};
	~Long_attr_with_wAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long_attr_with_w(att);}
};

class Short_attr_rwAttr: public Tango::Attr
{
public:
	Short_attr_rwAttr():Attr("Short_attr_rw", Tango::DEV_SHORT, Tango::READ_WRITE) {};
	~Short_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Short_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Short_attr_rw(att);}
};

class Long64_attr_rwAttr: public Tango::Attr
{
public:
	Long64_attr_rwAttr():Attr("Long64_attr_rw", Tango::DEV_LONG64, Tango::READ_WRITE) {};
	~Long64_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long64_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Long64_attr_rw(att);}
};

class ULong_attr_rwAttr: public Tango::Attr
{
public:
	ULong_attr_rwAttr():Attr("ULong_attr_rw", Tango::DEV_ULONG, Tango::READ_WRITE) {};
	~ULong_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_ULong_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_ULong_attr_rw(att);}
};

class ULong64_attr_rwAttr: public Tango::Attr
{
public:
	ULong64_attr_rwAttr():Attr("ULong64_attr_rw", Tango::DEV_ULONG64, Tango::READ_WRITE) {};
	~ULong64_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_ULong64_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_ULong64_attr_rw(att);}
};

class State_attr_rwAttr: public Tango::Attr
{
public:
	State_attr_rwAttr():Attr("State_attr_rw", Tango::DEV_STATE, Tango::READ_WRITE) {};
	~State_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_State_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_State_attr_rw(att);}
};


class Short_attr_wAttr: public Tango::Attr
{
public:
	Short_attr_wAttr():Attr("Short_attr_w", Tango::DEV_SHORT, Tango::WRITE) {set_memorized();}
	~Short_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Short_attr_w(att);}
};

class Short_attr_w2Attr: public Tango::Attr
{
public:
	Short_attr_w2Attr():Attr("Short_attr_w2", Tango::DEV_SHORT, Tango::WRITE) {}
	~Short_attr_w2Attr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Short_attr_w2(att);}
};

#ifndef COMPAT
class Encoded_attr_rwAttr: public Tango::Attr
{
public:
	Encoded_attr_rwAttr():Attr("Encoded_attr", Tango::DEV_ENCODED, Tango::READ_WRITE) {};
	~Encoded_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Encoded_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Encoded_attr_rw(att);}
};

class Encoded_attr_image: public Tango::Attr
{
public:
	Encoded_attr_image():Attr("Encoded_image", Tango::DEV_ENCODED, Tango::READ) {};
	~Encoded_attr_image() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Encoded_attr_image(att);}
};
#endif

//  --------------------------------------------------------------------------------

class Long_attr_wAttr: public Tango::Attr
{
public:
	Long_attr_wAttr():Attr("Long_attr_w", Tango::DEV_LONG, Tango::WRITE) {};
	~Long_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Long_attr_w(att);}
};

//  -------------------------------------------------------------------------------

class Double_attr_wAttr: public Tango::Attr
{
public:
	Double_attr_wAttr():Attr("Double_attr_w", Tango::DEV_DOUBLE, Tango::WRITE) {};
	~Double_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Double_attr_w(att);}
};

class String_attr_wAttr: public Tango::Attr
{
public:
	String_attr_wAttr():Attr("String_attr_w", Tango::DEV_STRING, Tango::WRITE) {set_memorized();}
	~String_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_String_attr_w(att);}
};

class String_attr_w2Attr: public Tango::Attr
{
public:
	String_attr_w2Attr():Attr("String_attr_w2", Tango::DEV_STRING, Tango::WRITE) {}
	~String_attr_w2Attr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_String_attr_w2(att);}
};

//  ---------------------------------------------------------------------------------

class Float_attr_wAttr: public Tango::Attr
{
public:
	Float_attr_wAttr():Attr("Float_attr_w", Tango::DEV_FLOAT, Tango::WRITE) {};
	~Float_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Float_attr_w(att);}
};

class Boolean_attr_wAttr: public Tango::Attr
{
public:
	Boolean_attr_wAttr():Attr("Boolean_attr_w", Tango::DEV_BOOLEAN, Tango::WRITE) {set_memorized();}
	~Boolean_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Boolean_attr_w(att);}
};

class UShort_attr_wAttr: public Tango::Attr
{
public:
	UShort_attr_wAttr():Attr("UShort_attr_w", Tango::DEV_USHORT, Tango::WRITE) {}
	~UShort_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_UShort_attr_w(att);}
};

class UChar_attr_wAttr: public Tango::Attr
{
public:
	UChar_attr_wAttr():Attr("UChar_attr_w", Tango::DEV_UCHAR, Tango::WRITE) {}
	~UChar_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_UChar_attr_w(att);}
};


//  ---------------------------------------------------------------------------------


class Float_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	Float_spec_attr_wAttr():SpectrumAttr("Float_spec_attr_w", Tango::DEV_FLOAT,Tango::WRITE, 4) {};
	~Float_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Float_spec_attr_w(att);}
};

class Boolean_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	Boolean_spec_attr_wAttr():SpectrumAttr("Boolean_spec_attr_w", Tango::DEV_BOOLEAN,Tango::WRITE, 4) {};
	~Boolean_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Boolean_spec_attr_w(att);}
};

class UShort_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	UShort_spec_attr_wAttr():SpectrumAttr("UShort_spec_attr_w", Tango::DEV_USHORT,Tango::WRITE, 4) {};
	~UShort_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_UShort_spec_attr_w(att);}
};

class UChar_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	UChar_spec_attr_wAttr():SpectrumAttr("UChar_spec_attr_w", Tango::DEV_UCHAR,Tango::WRITE, 4) {};
	~UChar_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_UChar_spec_attr_w(att);}
};


//  ---------------------------------------------------------------------------------


class attr_asynAttr: public Tango::Attr
{
public:
	attr_asynAttr():Attr("attr_asyn",Tango::DEV_DOUBLE) {};
	~attr_asynAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_asyn(att);}
};

class attr_asyn_toAttr: public Tango::Attr
{
public:
	attr_asyn_toAttr():Attr("attr_asyn_to",Tango::DEV_DOUBLE) {};
	~attr_asyn_toAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_asyn_to(att);}
};

class attr_asyn_exceptAttr: public Tango::Attr
{
public:
	attr_asyn_exceptAttr():Attr("attr_asyn_except",Tango::DEV_DOUBLE) {};
	~attr_asyn_exceptAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_asyn_except(att);}
};

class attr_asyn_writeAttr: public Tango::Attr
{
public:
	attr_asyn_writeAttr():Attr("attr_asyn_write",Tango::DEV_LONG, Tango::READ_WRITE) {};
	~attr_asyn_writeAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_attr_asyn_write(att);}
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_asyn_write(att);}
};

class attr_asyn_write_toAttr: public Tango::Attr
{
public:
	attr_asyn_write_toAttr():Attr("attr_asyn_write_to",Tango::DEV_LONG, Tango::WRITE) {};
	~attr_asyn_write_toAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_attr_asyn_write_to(att);}
};

class attr_asyn_write_exceptAttr: public Tango::Attr
{
public:
	attr_asyn_write_exceptAttr():Attr("attr_asyn_write_except",Tango::DEV_LONG, Tango::WRITE) {};
	~attr_asyn_write_exceptAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_attr_asyn_write_except(att);}
};

//  -----------------------------------------------------------------------------------

class PollLong_attrAttr: public Tango::Attr
{
public:
	PollLong_attrAttr():Attr("PollLong_attr", Tango::DEV_LONG, Tango::READ) {};
	~PollLong_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_PollLong_attr(att);}
};

class PollString_spec_attrAttr: public Tango::SpectrumAttr
{
public:
	PollString_spec_attrAttr():SpectrumAttr("PollString_spec_attr", Tango::DEV_STRING, 2) {};
	~PollString_spec_attrAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_PollString_spec_attr(att);}
};

//  ------------------------------------------------------------------------------------

class attr_dq_shAttr: public Tango::Attr
{
public:
	attr_dq_shAttr():Attr("attr_dq_sh",Tango::DEV_SHORT) {};
	~attr_dq_shAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_dq_sh(att);}
};

class attr_dq_loAttr: public Tango::Attr
{
public:
	attr_dq_loAttr():Attr("attr_dq_lo",Tango::DEV_LONG) {};
	~attr_dq_loAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_dq_lo(att);}
};

class attr_dq_dbAttr: public Tango::Attr
{
public:
	attr_dq_dbAttr():Attr("attr_dq_db",Tango::DEV_DOUBLE) {};
	~attr_dq_dbAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_dq_db(att);}
};

class attr_dq_strAttr: public Tango::Attr
{
public:
	attr_dq_strAttr():Attr("attr_dq_str",Tango::DEV_STRING) {};
	~attr_dq_strAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_attr_dq_str(att);}
};

//  ---------------------------------------------------------------------------------

class Short_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	Short_spec_attr_wAttr():SpectrumAttr("Short_spec_attr_w", Tango::DEV_SHORT,Tango::WRITE, 4) {};
	~Short_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Short_spec_attr_w(att);}
};

//  --------------------------------------------------------------------------------------

class Long_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	Long_spec_attr_wAttr():SpectrumAttr("Long_spec_attr_w", Tango::DEV_LONG,Tango::WRITE, 2) {};
	~Long_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Long_spec_attr_w(att);}
};

class Double_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	Double_spec_attr_wAttr():SpectrumAttr("Double_spec_attr_w", Tango::DEV_DOUBLE,Tango::WRITE, 2) {};
	~Double_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Double_spec_attr_w(att);}
};

class String_spec_attr_wAttr: public Tango::SpectrumAttr
{
public:
	String_spec_attr_wAttr():SpectrumAttr("String_spec_attr_w", Tango::DEV_STRING,Tango::WRITE, 2) {};
	~String_spec_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_String_spec_attr_w(att);}
};

class Short_ima_attr_wAttr: public Tango::ImageAttr
{
public:
	Short_ima_attr_wAttr():ImageAttr("Short_ima_attr_w", Tango::DEV_SHORT,Tango::WRITE, 4,4) {};
	~Short_ima_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Short_ima_attr_w(att);}
};

class String_ima_attr_wAttr: public Tango::ImageAttr
{
public:
	String_ima_attr_wAttr():ImageAttr("String_ima_attr_w", Tango::DEV_STRING, Tango::WRITE,2,2) {};
	~String_ima_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_String_ima_attr_w(att);}
};

class Float_ima_attr_wAttr: public Tango::ImageAttr
{
public:
	Float_ima_attr_wAttr():ImageAttr("Float_ima_attr_w", Tango::DEV_FLOAT,Tango::WRITE, 8,8) {};
	~Float_ima_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Float_ima_attr_w(att);}
};

class UShort_ima_attr_wAttr: public Tango::ImageAttr
{
public:
	UShort_ima_attr_wAttr():ImageAttr("UShort_ima_attr_w", Tango::DEV_USHORT, Tango::WRITE,8,2) {};
	~UShort_ima_attr_wAttr() {};

	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_UShort_ima_attr_w(att);}
};

//  -----------------------------------------------------------------------------------

class Short_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	Short_spec_attr_rwAttr():SpectrumAttr("Short_spec_attr_rw", Tango::DEV_SHORT,Tango::READ_WRITE, 4) {};
	~Short_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Short_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Short_spec_attr_rw(att);}
};

class String_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	String_spec_attr_rwAttr():SpectrumAttr("String_spec_attr_rw", Tango::DEV_STRING,Tango::READ_WRITE, 2) {};
	~String_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_String_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_String_spec_attr_rw(att);}
};

class Long_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	Long_spec_attr_rwAttr():SpectrumAttr("Long_spec_attr_rw", Tango::DEV_LONG,Tango::READ_WRITE, 3) {};
	~Long_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Long_spec_attr_rw(att);}
};

class Short_ima_attr_rwAttr: public Tango::ImageAttr
{
public:
	Short_ima_attr_rwAttr():ImageAttr("Short_ima_attr_rw", Tango::DEV_SHORT,Tango::READ_WRITE, 4,4) {};
	~Short_ima_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Short_ima_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Short_ima_attr_rw(att);}
};

class String_ima_attr_rwAttr: public Tango::ImageAttr
{
public:
	String_ima_attr_rwAttr():ImageAttr("String_ima_attr_rw", Tango::DEV_STRING,Tango::READ_WRITE, 2,2) {};
	~String_ima_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_String_ima_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_String_ima_attr_rw(att);}
};


//  --------------------------------------------------------------------------------------

class Float_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	Float_spec_attr_rwAttr():SpectrumAttr("Float_spec_attr_rw", Tango::DEV_FLOAT,Tango::READ_WRITE, 4) {};
	~Float_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Float_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Float_spec_attr_rw(att);}
};

class UChar_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	UChar_spec_attr_rwAttr():SpectrumAttr("UChar_spec_attr_rw", Tango::DEV_UCHAR,Tango::READ_WRITE, 4) {};
	~UChar_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_UChar_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_UChar_spec_attr_rw(att);}
};

//  -----------------------------------------------------------------------------------

class Boolean_ima_attr_rwAttr: public Tango::ImageAttr
{
public:
	Boolean_ima_attr_rwAttr():ImageAttr("Boolean_ima_attr_rw", Tango::DEV_BOOLEAN,Tango::READ_WRITE, 4,4) {};
	~Boolean_ima_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Boolean_ima_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Boolean_ima_attr_rw(att);}
};

class UShort_ima_attr_rwAttr: public Tango::ImageAttr
{
public:
	UShort_ima_attr_rwAttr():ImageAttr("UShort_ima_attr_rw", Tango::DEV_USHORT,Tango::READ_WRITE, 2,4) {};
	~UShort_ima_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_UShort_ima_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_UShort_ima_attr_rw(att);}
};


//  -----------------------------------------------------------------------------------

class Event_change_tstAttr: public Tango::SpectrumAttr
{
public:
	Event_change_tstAttr():SpectrumAttr("Event_change_tst", Tango::DEV_LONG,Tango::READ, 10) {};
	~Event_change_tstAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Event_change_tst(att);}
};

class Event64_change_tstAttr: public Tango::SpectrumAttr
{
public:
	Event64_change_tstAttr():SpectrumAttr("Event64_change_tst", Tango::DEV_LONG64,Tango::READ, 6) {};
	~Event64_change_tstAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Event64_change_tst(att);}
};


class Event_quality_tstAttr: public Tango::SpectrumAttr
{
public:
	Event_quality_tstAttr():SpectrumAttr("Event_quality_tst", Tango::DEV_DOUBLE,Tango::READ, 2) {};
	~Event_quality_tstAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Event_quality_tst(att);}
};

class Poll_buffAttr: public Tango::ImageAttr
{
public:
	Poll_buffAttr():ImageAttr("Poll_buff", Tango::DEV_STRING,Tango::READ,2,2) {};
	~Poll_buffAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Poll_buff(att);}
};

class Poll_buffRWAttr: public Tango::ImageAttr
{
public:
	Poll_buffRWAttr():ImageAttr("Poll_buffRW", Tango::DEV_STRING,Tango::READ_WRITE,2,2) {};
	~Poll_buffRWAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Poll_buffRW(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Poll_buffRW(att);}
};

// ----------------------------------------------------------------------------

class slow_actuatorAttr: public Tango::Attr
{
public:
	slow_actuatorAttr():Attr("slow_actuator",Tango::DEV_SHORT,Tango::READ_WRITE) {};
	~slow_actuatorAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_slow_actuator(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_slow_actuator(att);}
};


class fast_actuatorAttr: public Tango::Attr
{
public:
	fast_actuatorAttr():Attr("fast_actuator",Tango::DEV_DOUBLE,Tango::READ_WRITE) {};
	~fast_actuatorAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_fast_actuator(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_fast_actuator(att);}
};

// ----------------------------------------------------------------------------

class Long64_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	Long64_spec_attr_rwAttr():SpectrumAttr("Long64_spec_attr_rw", Tango::DEV_LONG64,Tango::READ_WRITE, 3) {};
	~Long64_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Long64_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Long64_spec_attr_rw(att);}
};

class ULong_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	ULong_spec_attr_rwAttr():SpectrumAttr("ULong_spec_attr_rw", Tango::DEV_ULONG,Tango::READ_WRITE, 3) {};
	~ULong_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_ULong_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_ULong_spec_attr_rw(att);}
};

class ULong64_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	ULong64_spec_attr_rwAttr():SpectrumAttr("ULong64_spec_attr_rw", Tango::DEV_ULONG64,Tango::READ_WRITE, 3) {};
	~ULong64_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_ULong64_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_ULong64_spec_attr_rw(att);}
};

class State_spec_attr_rwAttr: public Tango::SpectrumAttr
{
public:
	State_spec_attr_rwAttr():SpectrumAttr("State_spec_attr_rw", Tango::DEV_STATE,Tango::READ_WRITE, 3) {};
	~State_spec_attr_rwAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_State_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_State_spec_attr_rw(att);}
};



// ----------------------------------------------------------------------------

class Sub_device_tstAttr: public Tango::Attr
{
public:
	Sub_device_tstAttr():Attr("Sub_device_tst",Tango::DEV_BOOLEAN,Tango::READ) {};
	~Sub_device_tstAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Sub_device_tst(att);}
};

class SlowAttr: public Tango::Attr
{
public:
	SlowAttr():Attr("SlowAttr",Tango::DEV_DOUBLE,Tango::READ) {};
	~SlowAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Slow_attr(att);}
};


class DefAttr: public Tango::Attr
{
public:
	DefAttr():Attr("DefAttr",Tango::DEV_LONG,Tango::READ_WRITE) {};
	~DefAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Def_attr(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Def_attr(att);}
};

class DefUserAttr: public Tango::Attr
{
public:
	DefUserAttr():Attr("DefUserAttr",Tango::DEV_LONG,Tango::READ_WRITE) {};
	~DefUserAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_DefUser_attr(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_DefUser_attr(att);}
};

class DefClassAttr: public Tango::Attr
{
public:
	DefClassAttr():Attr("DefClassAttr",Tango::DEV_LONG,Tango::READ_WRITE) {};
	~DefClassAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_DefClass_attr(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_DefClass_attr(att);}
};

class DefClassUserAttr: public Tango::Attr
{
public:
	DefClassUserAttr():Attr("DefClassUserAttr",Tango::DEV_LONG,Tango::READ_WRITE) {};
	~DefClassUserAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_DefClassUser_attr(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_DefClassUser_attr(att);}
};

class ReynaldPollAttr: public Tango::Attr
{
public:
	ReynaldPollAttr():Attr("ReynaldPollAttr",Tango::DEV_DOUBLE,Tango::READ) {};
	~ReynaldPollAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_ReynaldPoll_attr(att);}
};

//-------------------------------------------------------------------------------------------------------

class EnumAttr: public Tango::Attr
{
public:
	EnumAttr():Attr("Enum_attr_rw",Tango::DEV_ENUM,Tango::READ_WRITE) {}
	~EnumAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Enum_attr(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Enum_attr(att);}

	virtual bool same_type(const type_info &in_type) {return typeid(CardinalPoints) == in_type;}
	virtual string get_enum_type() {return string("CardinalPoints");}
};

class EnumSpecAttr: public Tango::SpectrumAttr
{
public:
	EnumSpecAttr():SpectrumAttr("Enum_spec_attr_rw",Tango::DEV_ENUM,Tango::READ_WRITE,5) {};
	~EnumSpecAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_Enum_spec_attr_rw(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<DevTest *>(dev))->write_Enum_spec_attr_rw(att);}

	virtual bool same_type(const type_info &in_type) {return typeid(CardinalPoints) == in_type;}
	virtual string get_enum_type() {return string("CardinalPoints");}
};

class DynEnumAttr: public Tango::Attr
{
public:
	DynEnumAttr():Attr("DynEnum_attr", Tango::DEV_ENUM,Tango::READ) {};
	~DynEnumAttr() {};

	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<DevTest *>(dev))->read_DynEnum_attr(att);}
};

//	Pipe classes declaration

class RPipeClass : public Tango::Pipe
{
public:
	RPipeClass(const string &name,Tango::DispLevel  level)
	:Pipe(name,level) {};

	~RPipeClass() {};

	virtual bool is_allowed (Tango::DeviceImpl *dev,Tango::PipeReqType _prt)
		{return (static_cast<DevTest *>(dev))->is_RPipe_allowed(_prt);}
	virtual void read(Tango::DeviceImpl *dev)
		{(static_cast<DevTest *>(dev))->read_RPipe(*this);}
};

class RWPipeClass : public Tango::WPipe
{
public:
	RWPipeClass(const string &name,Tango::DispLevel  level)
	:WPipe(name,level) {};

	~RWPipeClass() {};

	virtual bool is_allowed (Tango::DeviceImpl *dev,Tango::PipeReqType _prt)
		{return (static_cast<DevTest *>(dev))->is_RWPipe_allowed(_prt);}
	virtual void read(Tango::DeviceImpl *dev)
		{(static_cast<DevTest *>(dev))->read_RWPipe(*this);}
	virtual void write(Tango::DeviceImpl *dev)
		{(static_cast<DevTest *>(dev))->write_RWPipe(*this);}
};

class ConfPipeClass : public Tango::Pipe
{
public:
	ConfPipeClass(const string &name,Tango::DispLevel  level)
	:Pipe(name,level) {};

	~ConfPipeClass() {};

	virtual bool is_allowed (Tango::DeviceImpl *dev,Tango::PipeReqType _prt)
		{return (static_cast<DevTest *>(dev))->is_ConfPipe_allowed(_prt);}
	virtual void read(Tango::DeviceImpl *dev)
		{(static_cast<DevTest *>(dev))->read_ConfPipe(*this);}
};

class RPipeDEClass : public Tango::Pipe
{
public:
	RPipeDEClass(const string &name,Tango::DispLevel  level)
	:Pipe(name,level) {};

	~RPipeDEClass() {};

	virtual bool is_allowed (Tango::DeviceImpl *dev,Tango::PipeReqType _prt)
		{return (static_cast<DevTest *>(dev))->is_RPipeDE_allowed(_prt);}
	virtual void read(Tango::DeviceImpl *dev)
		{(static_cast<DevTest *>(dev))->read_RPipeDE(*this);}
};

// ----------------------------------------------------------------------------


//
// The DevTestClass singleton definition
//

class DevTestClass : public Tango::DeviceClass
{
public:
	static DevTestClass *init(const char *);
	static DevTestClass *instance();
	~DevTestClass() {_instance = NULL;}

	virtual void device_name_factory(vector<string> &);
	virtual void signal_handler(long signo);

protected:
	DevTestClass(string &);
	static DevTestClass *_instance;
	void command_factory();
	void attribute_factory(vector<Tango::Attr *> &);
	void pipe_factory();

private:
	void device_factory(const Tango::DevVarStringArray *);
};

#endif // _DEV_TESTCLASS_H






