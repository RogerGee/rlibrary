#include "rutility.h"
using namespace rtypes;

size_type rtypes::rutil_strncpy(char* buffer,const char* source,size_type n)
{
    size_type i = 0;
    while (i < n)
    {
        if (source[i])
            buffer[i] = source[i];
        else
        {
            buffer[i] = 0;
            break;
        }
        ++i;
    }
    return i;
}
