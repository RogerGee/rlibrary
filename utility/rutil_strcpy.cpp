#include "rutility.h"
using namespace rtypes;

size_type rtypes::rutil_strcpy(char* buffer,const char* source)
{
    size_type i = 0;
    while (true)
    {
        buffer[i] = source[i];
        if (!buffer[i])
            break;
        ++i;
    }
    return i;
}
