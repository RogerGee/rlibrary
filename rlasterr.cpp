/* rlasterr.cpp
 *  Compile target framework flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */

#include "rlasterr.h"
using namespace rtypes;

// define target-specific code

#if defined(RLIBRARY_BUILD_POSIX)
#include "rlasterr_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "rlasterr_win32.cpp"
#endif

// define target-independent code

/* static */ rlib_error* rlib_last_error::_lastError = NULL;
/* static */ const rlib_error* rlib_last_error::get()
{
    // TODO: set error for each thread for each thread
    return _lastError;
}
/* static */ void rlib_last_error::set(const char* message,int code)
{
    // TODO: manage static data for each thread
    static byte errObj[ sizeof(rlib_error_message) ];
    new (errObj) rlib_error_message(message,code);
    _set( reinterpret_cast<rlib_error_message*>(errObj) );
}
/* static */ void rlib_last_error::_set(rlib_error* perr)
{
    _lastError = perr;
}
/* static */ void rlib_last_error::_sysSet(int code)
{
    static byte errObj[ sizeof(rlib_system_error) ];
    new (errObj) rlib_system_error(code);
    _set( reinterpret_cast<rlib_system_error*>(errObj) );
}
