//=============================================================================	
//
// file :		attrdesc.h
//
// description :	Include file for the Attr classes hierarchy. 
//			Three classes are declared in this file :
//				The Attr class
//				The SpectrumAttr class
//				The ImageAttr class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//=============================================================================

#ifndef _ATTRDESC_H
#define _ATTRDESC_H

#include <tango.h>

namespace Tango
{

class AttrProperty;

/**
 * User class to create a no dimension attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 * @author	$Author$
 * @version	$Revision$
 */
 
class Attr
{
public:

/**@name Constructors
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated Attr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	w_type	The attribute type (read, write, read with write ...)
 * @param	assoc	Name of the associated writable attribute. This is used
 * only the read with write attribute
 *
 */
	Attr(const char *name,long data_type,
	     Tango::AttrWriteType w_type = Tango::READ,
	     const char *assoc = AssocWritNotSpec);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
	virtual ~Attr();
//@}

	string 			&get_name() {return name;}
	Tango::AttrDataFormat 	get_format() {return format;}
	Tango::AttrWriteType 	get_writable() {return writable;}
	long 			get_type() {return type;}
	string			&get_assoc() {return assoc_name;}
	bool			is_assoc()
				{
					if (assoc_name != AssocWritNotSpec)
						return true;
					else
						return false;
				}
	
	vector<AttrProperty>	&get_class_properties() {return class_properties;}
	void 			set_class_properties(vector<AttrProperty> &in_prop)
				{
					class_properties = in_prop;
				}
	void			check_type();
	friend ostream &operator<<(ostream &,const Attr &);
		
protected:
	string			name;
	Tango::AttrDataFormat	format;
	Tango::AttrWriteType	writable;
	long			type;
	string			assoc_name;
	
	vector<AttrProperty>	class_properties;
};

/**
 * User class to create a one dimension attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 * @author	$Author$
 * @version	$Revision$
 */
 
class SpectrumAttr: public Attr
{
public:

/**@name Constructors
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated SpectrumAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 *
 */
	SpectrumAttr(const char *name,long data_type,long max_x);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
	~SpectrumAttr() {};
//@}
	
	long 			get_max_x() {return max_x;}
	
protected:
	long			max_x;
};

/**
 * User class to create a two dimensions attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 * @author	$Author$
 * @version	$Revision$
 */
 
class ImageAttr: public SpectrumAttr
{
public:

/**@name Constructors
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated ImageAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 * @param	max_y	The attribute maximum y dimension
 *
 */
	ImageAttr(const char *name,long data_type,long max_x, long max_y);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
	~ImageAttr() {};
//@}
	
	long 			get_max_y() {return max_y;}
	
protected:
	long			max_y;
};

} // End of Tango namespace

#endif /* _ATTRDESC_H */
