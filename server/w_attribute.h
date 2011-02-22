//=============================================================================	
//
// file :		w_attribute.h
//
// description :	Include file for the WAttribute classes. 
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2005/03/03 15:36:26  taurel
// - Change in attribute quality factor change event. It is now fired by the Attribute
// set_quailty() and set_value_date_quality() methods. For scannable device.
//
// Revision 1.2  2005/01/13 08:31:51  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 1.1.2.3  2004/10/27 05:59:47  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 1.1.2.2  2004/09/27 09:10:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 1.1.2.1  2004/09/15 06:48:06  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//=============================================================================

#ifndef _WATTRIBUTE_H
#define _WATTRIBUTE_H

#include <tango.h>
#include <attrdesc.h>
#include <functional>
#include <time.h>

namespace Tango
{

//=============================================================================
//
//			The WAttribute class
//
//
// description :	This class inherits the Attribute class. There is one
//			instance of this class for each writable attribute
//
//=============================================================================

class WAttributeExt
{
};

/**
 * This class represents a writable attribute. It inherits from the Attribute
 * class and only add what is specific to writable attribute.
 *
 * $Author$
 * $Revision$
 */
 
class WAttribute:public Attribute
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Create a new Writable Attribute object.
 *
 * @param prop_list The attribute properties list. Each property is an object
 * of the AttrProperty class
 * @param tmp_attr The temporary attribute object built from user parameters
 * @param dev_name The device name
 * @param idx The index of the related Attr object in the MultiClassAttribute
 *            vector of Attr object
 */
	WAttribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name,long idx);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class
 */
//@{
/**
 * The WAttribute desctructor.
 */
	~WAttribute(); 
//@}

/**@name Get new value for attribute
 * Miscellaneous method to retrieve from the WAttribute object the new value
 * for the attribute.
 */
 //@{
 /**
 * Retrieve the new value length (data number) for writable attribute.
 *
 * @return  The new value data length
 */
 	long get_write_value_length();

 /**
 * Retrieve the date of the last attribute writing. This is set only
 * if the attribute has a read different than set alarm. Otherwise,
 * date is set to 0.
 *
 * @return  The written date
 */
 	struct timeval &get_write_date() {return write_date;}
		
/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevShort.
 *
 * @param val A reference to a Tango::DevShort data which will be initialised
 * with the new value
 */	
	void get_write_value(Tango::DevShort &val) {val = short_val;}
	
/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevShort and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer wich will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::DevShort *&ptr) {ptr = short_ptr;}
	
/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevLong.
 *
 * @param val A reference to a Tango::DevLong data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevLong &val) {val = long_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevLong and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute 
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::DevLong *&ptr) {ptr = long_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevFloat.
 *
 * @param val A reference to a Tango::DevFloat data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevFloat &val) {val = float_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevFloat and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute 
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::DevFloat *&ptr) {ptr = float_ptr;}
		
/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevDouble.
 *
 * @param val A reference to a Tango::DevDouble data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevDouble &val) {val = double_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevDouble and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute 
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::DevDouble *&ptr) {ptr = double_ptr;}
	
/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevString.
 *
 * @param val A reference to a Tango::DevString data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevString &val) {val = str_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevString and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute 
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::ConstDevString *&ptr) {ptr = str_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevBoolean.
 *
 * @param val A reference to a Tango::DevBoolean data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevBoolean &val) {val = boolean_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevBoolean and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute 
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::DevBoolean *&ptr) {ptr = boolean_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevUShort.
 *
 * @param val A reference to a Tango::DevUShort data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevUShort &val) {val = ushort_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevUShort and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute 
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::DevUShort *&ptr) {ptr = ushort_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevUChar.
 *
 * @param val A reference to a Tango::DevUChar data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevUChar &val) {val = uchar_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevUChar and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute 
 * internal memory which must not be freed.
 */	
	void get_write_value(const Tango::DevUChar *&ptr) {ptr = uchar_ptr;}
		
//@}

/**@name Set new value for attribute
 * Miscellaneous method to set a WAttribute value
 */
 //@{
/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevShort.
 *
 * @param val A reference to a Tango::DevShort data 
 */	
	void set_write_value(Tango::DevShort val);

/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevLong.
 *
 * @param val A reference to a Tango::DevLong data 
 */
	void set_write_value(Tango::DevLong val);

/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevDouble.
 *
 * @param val A reference to a Tango::DevDouble
 */
	void set_write_value(Tango::DevDouble val);

/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevString.
 *
 * @param val A reference to a Tango::DevString 
 */
	void set_write_value(Tango::DevString val);
	
/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevString.
 *
 * @param val A reference to a std::string
 */
	void set_write_value(string &val);
	
/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevFloat.
 *
 * @param val A reference to a Tango::DevFloat
 */
	void set_write_value(Tango::DevFloat val);
	
/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevBoolean.
 *
 * @param val A reference to a Tango::DevBoolean
 */
	void set_write_value(Tango::DevBoolean val);
	
/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevUShort.
 *
 * @param val A reference to a Tango::DevUShort
 */
	void set_write_value(Tango::DevUShort val);
	
/**
 * Set the writable attribute value when attribute data type is 
 * Tango::DevUChar.
 *
 * @param val A reference to a Tango::DevUChar
 */
	void set_write_value(Tango::DevUChar val);
//@}	
	virtual void set_rvalue();
	
	void rollback();	
	void set_write_value(const CORBA::Any &,unsigned long,unsigned long);
	void copy_data(const CORBA::Any &);
	
	long get_w_dim_x() {return w_dim_x;}
	long get_w_dim_y() {return w_dim_y;}
	
	Tango::DevVarShortArray *get_last_written_sh() {return &short_array_val;}
	Tango::DevVarLongArray *get_last_written_lg() {return &long_array_val;}
	Tango::DevVarDoubleArray *get_last_written_db() {return &double_array_val;}
	Tango::DevVarStringArray *get_last_written_str() {return &str_array_val;}
	Tango::DevVarFloatArray *get_last_written_fl() {return &float_array_val;}
	Tango::DevVarBooleanArray *get_last_written_boo() {return &boolean_array_val;}
	Tango::DevVarUShortArray *get_last_written_ush() {return &ushort_array_val;}
	Tango::DevVarCharArray *get_last_written_uch() {return &uchar_array_val;}
	
	bool is_memorized() {return memorized;}
	void set_memorized(bool mem) {memorized = mem;}
	bool is_memorized_init() {return memorized_init;}
	void set_memorized_init(bool mem_init) {memorized_init = mem_init;}	
	string &get_mem_value() {return mem_value;}
	void set_written_date();

protected:
	virtual bool check_rds_alarm();
	
private:
// Defined prior to Tango IDL release 3
	
	Tango::DevShort 		short_val;
	Tango::DevShort			old_short_val;
	
	Tango::DevLong			long_val;
	Tango::DevLong			old_long_val;
	
	Tango::DevDouble		double_val;
	Tango::DevDouble		old_double_val;
	
	Tango::DevString		str_val;
	Tango::DevString		old_str_val;

	Tango::DevFloat			float_val;
	Tango::DevFloat			old_float_val;
	
	Tango::DevBoolean		boolean_val;
	Tango::DevBoolean		old_boolean_val;
	
	Tango::DevUShort		ushort_val;
	Tango::DevUShort		old_ushort_val;
	
	Tango::DevUChar			uchar_val;
	Tango::DevUChar			old_uchar_val;
		
// Added for Tango IDL release 3

	long				w_dim_y;
	long				w_dim_x;
	
	Tango::DevVarShortArray 	short_array_val;	
	Tango::DevVarLongArray		long_array_val;	
	Tango::DevVarDoubleArray 	double_array_val;	
	Tango::DevVarStringArray	str_array_val;
	Tango::DevVarFloatArray 	float_array_val;	
	Tango::DevVarBooleanArray	boolean_array_val;	
	Tango::DevVarUShortArray 	ushort_array_val;	
	Tango::DevVarCharArray		uchar_array_val;
		
	const Tango::DevShort  		*short_ptr;
	const Tango::DevLong  		*long_ptr;
	const Tango::DevDouble  	*double_ptr;
	const Tango::ConstDevString	*str_ptr;
	const Tango::DevFloat  		*float_ptr;
	const Tango::DevBoolean		*boolean_ptr;
	const Tango::DevUShort  	*ushort_ptr;
	const Tango::DevUChar		*uchar_ptr;
			
	bool				string_allocated;
	bool 				memorized;
	bool 				memorized_init;
	string				mem_value;
	struct timeval			write_date;
	WAttributeExt			*w_ext;
};


#define COMPUTE_TIME_DIFF(RESULT,BEFORE,AFTER) \
long bef = ((BEFORE.tv_sec - 1095000000) * 1000) + (BEFORE.tv_usec / 1000); \
long after = ((AFTER.tv_sec - 1095000000) * 1000) + (AFTER.tv_usec / 1000); \
RESULT = after - bef;

} // End of Tango namespace

#endif // _WATTRIBUTE_H
