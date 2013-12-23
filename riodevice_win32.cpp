/* riodevice_win32.cpp - implements riodevice using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include Windows API headers
#include <Windows.h>
using namespace rtypes;

// rtypes::io_resource
io_resource::io_resource()
    : _MyBase(INVALID_HANDLE_VALUE)
{
    _reference = 0;
}
io_resource::~io_resource()
{
    if (*this != INVALID_RESOURCE)
    {
        if ( ::CloseHandle( interpret_as<HANDLE>() )==0 )
            rlib_last_error::switch_throw();
        else
            this->assign(INVALID_HANDLE_VALUE);
    }
}

// rtypes::io_device
void io_device::_readBuffer(void* buffer,dword bytesToRead)
{
}
void io_device::_writeBuffer(const void* buffer,dword length)
{
}
