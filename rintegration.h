// rintegration.h - provides integration with the C++ Standard Library
#ifndef RINTEGRATION_H
#define RINTEGRATION_H
#include "rstring.h" // gets rtypestypes.h
#include <iostream>
#include <string>

namespace rtypes
{
    std::ostream& operator <<(std::ostream&,const str&);
    std::istream& operator >>(std::istream&,str&);
}

#endif
