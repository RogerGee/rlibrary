// rtypestypes.h - provides basic types for rlibrary
#ifndef RTYPESTYPES_H
#define RTYPESTYPES_H

// macros
#ifndef NULL
#define NULL 0
#endif

namespace rtypes
{
    // numeric types
    typedef unsigned char byte;
    typedef unsigned short word;
    typedef unsigned int dword;
    typedef unsigned long long qword;

    typedef dword size_type;
    //

    // enumerations
    //

    // structures (must not require external linkage)
    struct timestamp
    {
        word year; // year since 0000 AD
        word month; // month of year (1 - 12)
        word day; // day of month (1 - 31)
        word weekDay; // days since Sunday (0 - 6)
        word hour; // hours since midnight (0 - 23)
        word minute; // minutes after hour (0 - 59)
        word second; // seconds after minute (0 - 59)
        word millisecond; // milliseconds after second (0 - 999)
    };
    //
}

#endif
