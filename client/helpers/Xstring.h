#ifndef XSTRING_H
#define XSTRING_H

#include <sstream>
#include <string>

template<class T>   class XString{

public:	
	
	static	T convertFromString(const std::string& s)
	{
		std::istringstream in(s);
		T x;
		if (in >> x)
			return x;
		// some sort of error handling goes here...
		return 0;
	} 
//
	static	std::string convertToString(const T & t)
	{
		std::ostringstream out ;
		
		if (out << std::fixed << t   )
			return out.str();
		// some sort of error handling goes here...
		return 0;
	} 

};
#endif
