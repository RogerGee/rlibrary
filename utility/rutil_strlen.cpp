#include "rutility.h"
using namespace rtypes;

size_type rtypes::rutil_strlen(const char* pstring)
{
    size_type len = 0;
    while (pstring[len])
        ++len;
    return len;
}
