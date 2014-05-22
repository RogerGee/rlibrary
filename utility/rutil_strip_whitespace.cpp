#include "rutility.h"
using namespace rtypes;

str rtypes::rutil_strip_whitespace(const generic_string& item)
{
    str tmp;
    size_type i, j;
    if (item.length() > 0) {
        i = 0;
        while (i<item.length() && item[i]==' ')
            ++i;
        j = item.length();
        while (j>0 && item[j-1]==' ')
            --j;
        for (;i<j;i++)
            tmp.push_back(item[i]);
    }
    return tmp;
}

void rtypes::rutil_strip_whitespace_ref(generic_string& item)
{
    str tmp;
    size_type i, j;
    if (item.length() > 0) {
        i = 0;
        while (i<item.length() && item[i]==' ')
            ++i;
        j = item.length();
        while (j>0 && item[j-1]==' ')
            --j;
        if (i > 0) {
            for (;i<j;i++)
                tmp.push_back(item[i]);
            item = tmp;
        }
        else
            item.resize(j);
    }
}
