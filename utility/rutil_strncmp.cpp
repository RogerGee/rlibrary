#include "rutility.h"
using namespace rtypes;

// compare c-style strings up to n characters
bool rtypes::rutil_strncmp(const char* pa,const char* pb,size_type n)
{
    size_type i = 0;
    while (i<n && *pa && *pb && (*pa==*pb))
        ++i, ++pa, ++pb;
    return i == n;
}
