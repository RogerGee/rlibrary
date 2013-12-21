/* rlasterr_win32.cpp - implements rfilename using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

#include <Windows.h>
using namespace rtypes;

/* static */ void rlib_last_error::switch_set()
{
    DWORD err = ::GetLastError();
    // set documented errors
    if (err==ERROR_ACCESS_DENIED || err==ERROR_SHARING_VIOLATION)
        set<access_denied_error>();
    else if (err == ERROR_FILE_NOT_FOUND)
        set<does_not_exist_error>();
    else if (err == ERROR_DIR_NOT_EMPTY)
        set<directory_not_empty_error>();
    else if (err == ERROR_INVALID_HANDLE)
        set<invalid_resource_error>();
    // set undocumented error
    else
        set<rlib_error>(err);
}
/* static */ void rlib_last_error::switch_throw()
{
    DWORD err = ::GetLastError();
    // throw documented errors
    switch (err)
    {
    case ERROR_ACCESS_DENIED:
    case ERROR_SHARING_VIOLATION:
        throw access_denied_error();
    case ERROR_FILE_NOT_FOUND:
        throw does_not_exist_error();
    case ERROR_DIR_NOT_EMPTY:
        throw directory_not_empty_error();
    case ERROR_INVALID_HANDLE:
        throw invalid_resource_error();
    }
    // throw undocumented error
    throw rlib_error(err);
}

