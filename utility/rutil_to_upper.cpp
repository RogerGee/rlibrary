#include "rutility.h"
using namespace rtypes;

// return upper-case string variant
str rtypes::rutil_to_upper(const char* pstr)
{
    str result(pstr);
    for (size_type i = 0;i<result.length();i++)
        if (result[i]>='a' && result[i]<='z')
            result[i] -= 'a', result[i] += 'A';
    return result;
}
str rtypes::rutil_to_upper(const generic_string& sobj)
{
    str result(sobj);
    for (size_type i = 0;i<result.length();i++)
        if (result[i]>='a' && result[i]<='z')
            result[i] -= 'a', result[i] += 'A';
    return result;
}

// change string to upper-case variant
void rtypes::rutil_to_upper_ref(generic_string& sobj)
{
    for (size_type i = 0;i<sobj.length();i++)
        if (sobj[i]>='a' && sobj[i]<='z')
            sobj[i] -= 'a', sobj[i] += 'A';
}
