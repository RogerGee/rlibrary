/* rlasterr.h
 *  rlibrary/rlasterr.h - provides an interface for error handling, 
 * supplying a last error mechanism and cross-platform error-switching;
 * supports multiple thread support
 */
#ifndef RLASTERR_H
#define RLASTERR_H
#include "rerror.h" // gets rtypestypes.h
// TODO: include map functionality
// TODO: include threading functionality

namespace rtypes
{
    // error reason - gets the last error recorded by
    // another routine - used for non-exceptions
    class rlib_last_error
    {
    public:
        static const rlib_error* get();

        template<typename T>
        static void set();
        static void set(const char* message,int code = 1);

        static void switch_set();
        static void switch_throw();
    private:
        rlib_last_error();
        rlib_last_error& operator =(const rlib_last_error&);

        static void _set(rlib_error*);
        static void _sysSet(int code);

        // TODO: manage a last error for each thread
        static rlib_error* _lastError;
    };
}

template<typename T>
/* static */ void rtypes::rlib_last_error::set()
{
    // TODO: manage static data for each thread
    static byte errObj[ sizeof(T) ];
    new (errObj) T();
    // perform static type checking for rlib_error types
    _set( static_cast<rlib_error*> (reinterpret_cast<T*>(errObj)) );
}

#endif
