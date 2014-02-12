#include "rutility.h"
using namespace rtypes;

// return lower-case string variant
str rtypes::rutil_to_lower(const char* pstr)
{
    str result(pstr);
    for (size_type i = 0;i<result.length();i++)
        if (result[i]>='A' && result[i]<='Z')
            result[i] -= 'A', result[i] += 'a';
    return result;    
}
str rtypes::rutil_to_lower(const generic_string& sobj)
{
    str result(sobj);
    for (size_type i = 0;i<result.length();i++)
        if (result[i]>='A' && result[i]<='Z')
            result[i] -= 'A', result[i] += 'a';
    return result;
}

// change string to lower-case variant
void rtypes::rutil_to_lower_ref(generic_string& sobj)
{
    for (size_type i = 0;i<sobj.length();i++)
        if (sobj[i]>='A' && sobj[i]<='Z')
            sobj[i] -= 'A', sobj[i] += 'a';
}
