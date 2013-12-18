/* rutility.h
 *  rlibrary/rutility - contains utility functions used by rlibrary and its
 * implementation
 */
#ifndef RUTILITY_H
#define RUTILITY_H
#include "rtypestypes.h"

namespace rtypes
{
    // every utility function is prefixed 'rutil'
    void rutil_def_memory(void* pdata,dword size,byte val = 0); // fills specified data with specified value
    bool rutil_strcmp(const char*,const char*); // compares two c-style strings
}

#endif
