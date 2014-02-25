/* rlasterr_win32.cpp - implements rlasterr using the Windows API
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
    else if (err==ERROR_OUTOFMEMORY || err==ERROR_NOT_ENOUGH_MEMORY)
        set<out_of_memory_error>();
    else if (err == ERROR_BAD_ENVIRONMENT)
        set<error_bad_environment>();
    // set undocumented error
    else
        _sysSet(err);
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
    case ERROR_OUTOFMEMORY:
    case ERROR_NOT_ENOUGH_MEMORY:
        throw out_of_memory_error();
    case ERROR_BAD_ENVIRONMENT:
        throw bad_environment_error();
    }
    // throw undocumented error
    throw rlib_system_error(err);
}

