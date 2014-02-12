/* rutility.h
 *  rlibrary/rutility - contains utility functions used by rlibrary and its
 * implementation
 */
#ifndef RUTILITY_H
#define RUTILITY_H
#include "rtypestypes.h"
#include "rstring.h"

namespace rtypes
{
    // every utility function is prefixed 'rutil'
    void rutil_def_memory(void* pdata,size_type size,byte val = 0); // fills specified data with specified value
    bool rutil_strcmp(const char*,const char*); // compares two c-style strings
    size_type rutil_strlen(const char*); // returns length of c-style strings
    char* rutil_strcpy(char* buffer,const char* source); // copies source into buffer; returns buffer
    char* rutil_strncpy(char* buffer,const char* source,size_type n); // copies n-characters from source into buffer; copies extra null terminators; returns  buffer
    str rutil_to_lower(const char*); // return lower-case string variant
    str rutil_to_lower(const generic_string&);
    void rutil_to_lower_ref(generic_string&); // change string to lower-case variant
    str rutil_to_upper(const char*); // return upper-case string variant
    str rutil_to_upper(const generic_string&);
    void rutil_to_upper_ref(generic_string&); // change string to upper-case variant
}

#endif
