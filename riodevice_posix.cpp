/* riodevice_posix.cpp - implements riodevice using POSIX
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include POSIX and other system headers
#include <unistd.h>
using namespace rtypes;

// rtypes::io_resource
io_resource::io_resource()
    : _MyBase(-1)
{
    _reference = 0;
}
io_resource::~io_resource()
{
    if (*this != INVALID_RESOURCE)
    {
        if ( ::close( interpret_as<int>() ) != 0 )
            rlib_last_error::switch_throw();
        else
            this->assign(-1);
    }
}
