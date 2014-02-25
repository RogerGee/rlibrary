// rtypestypes.h - provides basic types for rlibrary
#ifndef RTYPESTYPES_H
#define RTYPESTYPES_H
#include <cstddef>
#include <new>

// macros
#ifndef NULL
#define NULL 0
#endif

namespace rtypes
{
    // numeric types
    typedef unsigned char byte;
    typedef short int16;
    typedef unsigned short uint16;
    typedef int int32;
    typedef unsigned int uint32;
    typedef long long int64;
    typedef unsigned long long uint64;

    typedef long long int ssize_type;
    typedef std::size_t size_type;
    //

    // enumerations
    //

    // structures (must not require external linkage)
    struct timestamp
    {
        uint16 year; // year since 0000 AD
        uint16 month; // month of year (1 - 12)
        uint16 day; // day of month (1 - 31)
        uint16 weekDay; // days since Sunday (0 - 6)
        uint16 hour; // hours since midnight (0 - 23)
        uint16 minute; // minutes after hour (0 - 59)
        uint16 second; // seconds after minute (0 - 59)
        uint16 millisecond; // milliseconds after second (0 - 999)
    };
    //
}

#endif
