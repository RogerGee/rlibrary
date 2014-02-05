#include "rutility.h"
using namespace rtypes;

void rtypes::rutil_def_memory(void* pdata,size_type size,byte val)
{
    byte* p = reinterpret_cast<byte*> (pdata);
    for (size_type cnt = 0;cnt<size;cnt++,p++)
        *p = val;
}
