/* rstdio_posix.cpp - implements rstdio using POSIX
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include POSIX and other system headers
#include <unistd.h>
using namespace rtypes;

// rtypes::standard_device
bool standard_device::open_error(const char*)
{
    _error = new io_resource( reinterpret_cast<void*>(STDERR_FILENO),false );
    return true;
}
void standard_device::_writeErrBuffer(const void* buffer,dword length)
{
    if (_error != NULL)
    {
        ssize_t bytesWrote;
        bytesWrote = ::write(_error->interpret_as<int>(),buffer,length);
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
void standard_device::_openEvent(const char*,io_access_flag kind,dword**,dword)
{
    if (kind & read_access)
        _input = new io_resource( reinterpret_cast<void*>(STDIN_FILENO),false );
    if (kind & write_access)
        _output = new io_resource( reinterpret_cast<void*>(STDOUT_FILENO),false );
    if (kind == all_access)
        _error = new io_resource( reinterpret_cast<void*>(STDERR_FILENO),false );
}
