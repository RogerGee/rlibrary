/* rutility.h
 *  rlibrary/rutility - contains utility functions used by rlibrary and its
 * implementation; some of these are based of C-library routines and provide
 * a C++-style interface to them
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
    bool rutil_strncmp(const char*,const char*,size_type n); // compares two c-style strings up to the specified number of characters
    size_type rutil_strlen(const char*); // returns length of c-style strings
    size_type rutil_strcpy(char* buffer,const char* source); // copies source into buffer; returns number of copied chars
    size_type rutil_strncpy(char* buffer,const char* source,size_type n); // copies n-chars or strlen(source) from source into buffer; returns number of copied chars
    str rutil_to_lower(const char*); // return lower-case string variant
    str rutil_to_lower(const generic_string&);
    void rutil_to_lower_ref(generic_string&); // change string to lower-case variant
    str rutil_to_upper(const char*); // return upper-case string variant
    str rutil_to_upper(const generic_string&);
    void rutil_to_upper_ref(generic_string&); // change string to upper-case variant
    str rutil_strip_whitespace(const generic_string&); // return string variant minus leading and trailing whitespace
    void rutil_strip_whitespace_ref(generic_string&); // change string to variant minus leading and trailing whitespace
}

#endif
