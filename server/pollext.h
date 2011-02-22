//=============================================================================
//
// file :               PollExt.h
//
// description :        Include for classes used by the method dedicated
//			   to fill the polling buffer for command or
//			   attributes.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.1.2.2  2004/08/26 07:34:45  taurel
// - Implement a way to directly fills command or attribute polling buffer
//
// Revision 1.1.2.1  2004/07/15 15:05:03  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _POLLEXT_H
#define _POLLEXT_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			Attribute related class
//
// description :	These classes are used when the user want to fill
//		       attribute polling buffer
//
//=============================================================================

//=============================================================================
//
//			The AttrData class
//
//
// description : 	This class is used to store all the data needed to build
//			an attribute value.	
//
//=============================================================================
 
template <typename T>
class AttrData
{
public:	
	const T 		*ptr;
	Tango::AttrQuality	qual;
	long			x;
	long			y;
	bool			release;
	DevErrorList		err;
 	
	AttrData(const T *p): ptr(p)
	{qual=Tango::ATTR_VALID;x=1;y=0;release=false;}
	
	AttrData(const T *p,Tango::AttrQuality q): ptr(p),qual(q)
	{x=1;y=0;release=false;}

	AttrData(const T *p,Tango::AttrQuality q,bool rel): ptr(p),qual(q),release(rel)
	{x=1;y=0;}

// For spectrum

	AttrData(const T *p,long nb): ptr(p),x(nb)
	{qual=Tango::ATTR_VALID;y=0;release=false;}	

	AttrData(const T *p,long nb,Tango::AttrQuality q): ptr(p),qual(q),x(nb)
	{y=0;release=false;}
	
	AttrData(const T *p,long nb,Tango::AttrQuality q,bool rel): ptr(p),qual(q),x(nb),release(rel)
	{y=0;}

// For image
	
	AttrData(const T *p,long nb,long nb2): ptr(p),x(nb),y(nb2)
	{qual=Tango::ATTR_VALID;release=false;}	

	AttrData(const T *p,long nb,long nb2,Tango::AttrQuality q): ptr(p),qual(q),x(nb),y(nb2)
	{release=false;}
	
	AttrData(const T *p,long nb,long nb2,Tango::AttrQuality q,bool rel): ptr(p),qual(q),
									     x(nb),y(nb2),release(rel)
	{}

// For error
	
	AttrData(DevErrorList &e): err(e)
	{}
};

//=============================================================================
//
//			The TimedAttrData class
//
//
// description : 	This class inherits from the AttrData class and adds 
//			a date to all the data contains in the AttrData
//			class
//
//=============================================================================

/**
 * This class is used to store one element of an attribute history stack.
 *
 * $Author$
 * $Revision$
 */
 
template <typename T>
class TimedAttrData:public Tango::AttrData<T>
{
public:
	struct timeval		t_val;

/**@name Miscellaneous constructors for scalar attribute
 */
//@{
/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 * The attribute quality factor will be set to ATTR_VALID
 *
 * @param p_data Pointer to the attribute value
 * @param when The date
 */	
	TimedAttrData(const T *p_data,time_t when): AttrData<T>(p_data) {t_val.tv_sec = when;t_val.tv_usec = 0;}	

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the attribute value
 * @param qual The attribute quality factor
 * @param when The date
 */
	TimedAttrData(const T *p_data,Tango::AttrQuality qual,time_t when): AttrData<T>(p_data,qual) {t_val.tv_sec = when;t_val.tv_usec = 0;}

/**
 * Create a new TimedAttrData object.
 *
 * @param p_data Pointer to the attribute value
 * @param qual The attribute quality factor
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed 
 * @param when The date
 */
	TimedAttrData(const T *p_data,Tango::AttrQuality qual,bool rel,time_t when): AttrData<T>(p_data,qual,rel)
	{t_val.tv_sec = when;t_val.tv_usec = 0;}

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 * The attribute quality factor will be set to ATTR_VALID
 *
 * @param p_data Pointer to the attribute value
 * @param when The date
 */
	TimedAttrData(const T *p_data,struct timeval when): AttrData<T>(p_data),t_val(when) {}	

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the attribute value
 * @param qual The attribute quality factor
 * @param when The date
 */
	TimedAttrData(const T *p_data,Tango::AttrQuality qual,struct timeval when): AttrData<T>(p_data,qual),t_val(when) {}

/**
 * Create a new TimedAttrData object.
 *
 * @param p_data Pointer to the attribute value
 * @param qual The attribute quality factor
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed 
 * @param when The date
 */
	TimedAttrData(const T *p_data,Tango::AttrQuality qual,bool rel,struct timeval when): AttrData<T>(p_data,qual,rel),t_val(when)
	{}
//@}


// For spectrum


/**@name Miscellaneous constructors for spectrum attribute
 */
//@{
/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 * The attribute quality factor will be set to ATTR_VALID
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,time_t when): AttrData<T>(p_data,x)
	{t_val.tv_sec = when;t_val.tv_usec = 0;}	

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param qual The attribute quality factor
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,Tango::AttrQuality qual,time_t when): AttrData<T>(p_data,x,qual)
	{t_val.tv_sec = when;t_val.tv_usec = 0;}

/**
 * Create a new TimedAttrData object.
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param qual The attribute quality factor
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed 
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,Tango::AttrQuality qual,bool rel,time_t when): AttrData<T>(p_data,x,qual,rel)
	{t_val.tv_sec = when;t_val.tv_usec = 0;}
	
/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 * The attribute quality factor will be set to ATTR_VALID
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param when The date
 */	
	TimedAttrData(const T *p_data,long x,struct timeval when): AttrData<T>(p_data,x),t_val(when) {}	

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param qual The attribute quality factor
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,Tango::AttrQuality qual,struct timeval when): AttrData<T>(p_data,x,qual),t_val(when) {}

/**
 * Create a new TimedAttrData object.
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param qual The attribute quality factor
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed 
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,Tango::AttrQuality qual,bool rel,struct timeval when): AttrData<T>(p_data,x,qual,rel),t_val(when)
	{}
//@}

	
// For image


/**@name Miscellaneous constructors for image attribute
 */
//@{
/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 * The attribute quality factor will be set to ATTR_VALID
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param y The attribute y length
 * @param when The date
 */	
	TimedAttrData(const T *p_data,long x,long y,time_t when): AttrData<T>(p_data,x,y) {t_val.tv_sec = when;t_val.tv_usec = 0;}  

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param y The attribute y length
 * @param qual The attribute quality factor
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,long y,Tango::AttrQuality qual,time_t when): AttrData<T>(p_data,x,y,qual)
	{t_val.tv_sec = when;t_val.tv_usec = 0;}

/**
 * Create a new TimedAttrData object.
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param y The attribute y length
 * @param qual The attribute quality factor
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed 
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,long y,Tango::AttrQuality qual,bool rel,time_t when): AttrData<T>(p_data,x,y,qual,rel)
	{t_val.tv_sec = when;t_val.tv_usec = 0;}

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 * The attribute quality factor will be set to ATTR_VALID
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param y The attribute y length
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,long y,struct timeval when): AttrData<T>(p_data,x,y),t_val(when) {}	

/**
 * Create a new TimedAttrData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param y The attribute y length
 * @param qual The attribute quality factor
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,long y,Tango::AttrQuality qual,struct timeval when): AttrData<T>(p_data,x,y,qual),t_val(when) {}

/**
 * Create a new TimedAttrData object.
 *
 * @param p_data Pointer to the attribute value
 * @param x The attribute x length
 * @param y The attribute y length
 * @param qual The attribute quality factor
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed 
 * @param when The date
 */
	TimedAttrData(const T *p_data,long x,long y,Tango::AttrQuality qual,bool rel,struct timeval when): AttrData<T>(p_data,x,y,qual,rel),t_val(when)
	{}
//@}


	
// For error

	
/**@name Miscellaneous constructors for errors
 */
//@{
/**
 * Create a new TimedAttrData object for errors.
 *
 * The created TimedAttrData is used to store attribute errors
 * in the attribute history stack
 *
 * @param errs The error stack
 * @param when The date
 */	
	TimedAttrData(DevErrorList &errs,time_t when): AttrData<T>(errs) {t_val.tv_sec = when;t_val.tv_usec = 0;}

/**
 * Create a new TimedAttrData object for errors.
 *
 * The created TimedAttrData is used to store attribute errors
 * in the attribute history stack
 *
 * @param errs The error stack
 * @param when The date
 */
	TimedAttrData(DevErrorList &errs,timeval when): AttrData<T>(errs),t_val(when) {}
//@}
	
#ifdef WIN32

	TimedAttrData(const T *p,struct _timeb t): AttrData<T>(p) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}	
	TimedAttrData(const T *p,Tango::AttrQuality q,struct _timeb t): AttrData<T>(p,q) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}
	TimedAttrData(const T *p,Tango::AttrQuality q,bool rel,struct _timeb t): AttrData<T>(p,q,rel)
	{t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}

	TimedAttrData(const T *p,long nb,struct _timeb t): AttrData<T>(p,nb) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}	
	TimedAttrData(const T *p,long nb,Tango::AttrQuality q,struct _timeb t): AttrData<T>(p,nb,q) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}
	TimedAttrData(const T *p,long nb,Tango::AttrQuality q,bool rel,struct _timeb t): AttrData<T>(p,nb,q,rel)
	{t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}

	TimedAttrData(const T *p,long nb,long nb2,struct _timeb t): AttrData<T>(p,nb,nb2) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}	
	TimedAttrData(const T *p,long nb,long nb2,Tango::AttrQuality q,struct _timeb t): AttrData<T>(p,nb,nb2,q) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}
	TimedAttrData(const T *p,long nb,long nb2,Tango::AttrQuality q,bool rel,struct _timeb t): AttrData<T>(p,nb,nb2,q,rel)
	{t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}

#endif

};


//=============================================================================
//
//			The AttrHistoryStack class
//
//
// description : 	This class is simply a wrapper above a vector of
//			TimedAttrData class. It is  used to pass an attribute
//			value history which will be stored in the polling
//			buffer
//
//=============================================================================

/**
 * This class is a used to pass an attribute value history when the user
 * directly fills the attribute polling buffer. Each element in this stack
 * will be used to store one element of the attribute polling buffer
 *
 * $Author$
 * $Revision$
 */
 
template <typename T>
class AttrHistoryStack
{
public:
	AttrHistoryStack() {};

/**
 * Store a new element in the stack
 *
 * This method stores a new element in the stack 
 *
 * @param elt The new element
 */	
	void push(TimedAttrData<T> const &elt) {hist.push_back(elt);}
	
/**
 * Get stack depth
 *
 * @return The stack depth
 */
	unsigned long length() {return hist.size();}
	
/**
 * Reserve memory for stack elements
 *
 * @param nb The stack element number
 */
	void length(long nb) {hist.reserve(nb);}

/**
 * Clear the stack
 */	
	void clear() {hist.clear();}
	
/**
 * Get stack data
 *
 * @return The stack itself
 */
	vector<TimedAttrData<T> > &get_data() {return hist;}
	
	vector<Tango::TimedAttrData<T> >	hist;
};


//=============================================================================
//
//			Command related class
//
// description :	These classes are used when the user want to fill
//		       command polling buffer
//
//=============================================================================


//=============================================================================
//
//			The TimedCmdData class
//
//
// description : 	This class is used to store all the data needed to build
//			a command value plus a date.	
//
//=============================================================================

/**
 * This class is used to store one element of a command history stack.
 *
 * $Author$
 * $Revision$
 */
 
template <typename T>
class TimedCmdData
{
public:	
	T		*ptr;
	DevErrorList	err;
	struct timeval	t_val;
	bool		release;

/**@name Constructors
 * Miscellaneous constructors
 */
//@{
/**
 * Create a new TimedCmdData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the command result data
 * @param when The date
 */
	TimedCmdData(T *p_data,time_t when): ptr(p_data),release(false) {t_val.tv_sec = when;t_val.tv_usec = 0;}  

/**
 * Create a new TimedCmdData object with memory management.
 *
 * @param p_data Pointer to the command result data
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed
 * @param when The date
 */

	TimedCmdData(T *p_data,bool rel,time_t when): ptr(p_data),release(rel)
	{t_val.tv_sec = when;t_val.tv_usec = 0;}

/**
 * Create a new TimedCmdData object.
 *
 * The memory pointed to by the <i>p_data</i> parameter will not be freed
 *
 * @param p_data Pointer to the command result data
 * @param when The date
 */
	TimedCmdData(T *p_data,struct timeval when): ptr(p_data),t_val(when) {}   

/**
 * Create a new TimedCmdData object with memory management.
 *
 * @param p_data Pointer to the command result data
 * @param rel Set to true if the memory pointed to by the <i>p_data</i> parameter must be freed
 * @param t The date
 */
	TimedCmdData(T *p_data,bool rel,struct timeval when): ptr(p_data),t_val(when),release(rel) {}

#ifdef WIN32
	TimedCmdData(T *p,struct _timeb t): ptr(p),release(false) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;} 
	TimedCmdData(T *p,bool rel,struct _timeb t): ptr(p),release(rel) {t_val.tv_sec = t.time;t_val.tv_usec = t.millitm*1000;}
#endif


/**
 * Create a new TimedCmdData object for errors.
 *
 * The created TimedCmdData is used to store command errors
 * in the command history stack
 *
 * @param errs The error stack
 * @param when The date
 */	
	TimedCmdData(DevErrorList errs,time_t when): err(errs) {t_val.tv_sec = when;t_val.tv_usec = 0;}
/**
 * Create a new TimedCmdData object for errors.
 *
 * The created TimedCmdData is used to store command errors
 * in the command history stack
 *
 * @param errs The error stack
 * @param when The date
 */
	TimedCmdData(DevErrorList errs,timeval when): err(errs),t_val(when) {}
	
//@}
};


//=============================================================================
//
//			The CmdHistoryStack class
//
//
// description : 	This class is simply a wrapper above a vector of
//			TimedCmdData class. It is  used to pass a command
//			value history which will be stored in the polling
//			buffer
//
//=============================================================================


/**
 * This class is a used to pass a command result history when the user
 * directly fills the command polling buffer. Each element in this stack
 * will be used to store one element of the command polling buffer
 *
 * $Author$
 * $Revision$
 */
 
template <typename T>
class CmdHistoryStack
{
public:
	CmdHistoryStack() {};

/**
 * Store a new element in the stack
 *
 * This method stores a new element in the stack 
 *
 * @param elt The new element
 */	
	void push(Tango::TimedCmdData<T> const &elt) {hist.push_back(elt);}
	
/**
 * Get stack depth
 *
 * @return The stack depth
 */	
	unsigned long length() {return hist.size();}
	
/**
 * Reserve memory for stack elements
 *
 * @param nb The stack element number
 */	
	void length(long nb) {hist.reserve(nb);}
	
/**
 * Clear the stack
 */	
	void clear() {hist.clear();}
	
/**
 * Get stack data
 *
 * @return The stack itself
 */
	vector<TimedCmdData<T> > &get_data() {return hist;}
	
	vector<Tango::TimedCmdData<T> >	hist;
};


} // End of Tango namespace

#endif /* _POLLOBJ_ */
