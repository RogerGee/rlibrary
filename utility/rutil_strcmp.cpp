#include "rutility.h"
using namespace rtypes;

// compare c-style strings
bool rtypes::rutil_strcmp(const char* pa,const char* pb)
{
    while (*pa && *pb && (*pa==*pb))
        ++pa, ++pb;
    return *pa==0 && *pb==0;
}
