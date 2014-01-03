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
    _closable = true;
}
io_resource::~io_resource()
{
    if (*this!=INVALID_RESOURCE && _closable)
    {
        if ( ::close( interpret_as<int>() ) != 0 )
            rlib_last_error::switch_throw();
        else
            this->assign(-1);
    }
}

// rtypes::io_device
void io_device::_readBuffer(void* buffer,dword bytesToRead)
{
    if (_input != NULL)
    {
        ssize_t bytesRead;
        bytesRead = ::read(_input->interpret_as<int>(),buffer,bytesToRead);
        if (bytesRead == 0)
        {
            _lastOp = no_input;
            _byteCount = 0;
        }
        else if (bytesRead <= -1)
        {
            _lastOp = bad_read;
            _byteCount = 0;
        }
        else
        {
            _lastOp = success_read;
            _byteCount = dword(bytesRead);
        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
void io_device::_writeBuffer(const void* buffer,dword length)
{
    if (_output != NULL)
    {
        ssize_t bytesWrote;
        bytesWrote = ::write(_output->interpret_as<int>(),buffer,length);
        if (bytesWrote == 0)
        {
            _lastOp = no_output;
            _byteCount = 0;
        }
        else if (bytesWrote <= -1)
        {
            _lastOp = bad_write;
            _byteCount = 0;
        }
        else
        {
            _lastOp = success_write;
            _byteCount = dword(bytesWrote);
        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
