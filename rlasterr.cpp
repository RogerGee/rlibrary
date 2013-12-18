/* rlasterr.cpp
 *  Compile target platform flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */

#include "rlasterr.h"

// define target-specific code
//  -> Imports: 'using namespace rtypes'

#if defined(RLIBRARY_BUILD_POSIX)
#include "rlasterr_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "rlasterr_win32.cpp"
#else
using namespace rtypes;
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
    static byte errObj[ sizeof(rlib_err_message) ];
    new (errObj) rlib_err_message(message,code);
    _set( reinterpret_cast<rlib_err_message*>(errObj) );
}
/* static */ void rlib_last_error::_set(rlib_error* perr)
{
    _lastError = perr;
}
