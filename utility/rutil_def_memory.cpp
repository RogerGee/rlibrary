#include "../rutility.h"
using namespace rtypes;

void rtypes::rutil_def_memory(void* pdata,dword size,byte val)
{
    byte* p = reinterpret_cast<byte*> (pdata);
    for (dword cnt = 0;cnt<size;cnt++,p++)
        *p = val;
}
