/* rstdio_win32.cpp - implements rstdio using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include Windows API headers
#include <Windows.h>
using namespace rtypes;

// rtypes::standard_device
bool standard_device::open_error(const char*)
{
    HANDLE hStdHandle;
    hStdHandle = ::GetStdHandle(STD_ERROR_HANDLE);
    _error = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    return true;
}
void standard_device::_writeErrBuffer(const void* buffer,dword length)
{
    if (_error != NULL)
    {
        DWORD bytesWrote;
        if ( !::WriteFile(_error->interpret_as<HANDLE>(),buffer,length,&bytesWrote,NULL) )
        {
            _lastOp = bad_write;
            _byteCount = 0;
        }
        else if (bytesWrote == 0)
        {
            _lastOp = no_output;
            _byteCount = 0;
        }
        else
        {
            _lastOp = success_write;
            _byteCount = bytesWrote;

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
    HANDLE hStdHandle;
    if (kind & read_access)
    {
        hStdHandle = ::GetStdHandle(STD_INPUT_HANDLE);
        _input = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    }
    if (kind & write_access)
    {
        hStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        _output = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    }
    if (kind == all_access)
    {
        hStdHandle = ::GetStdHandle(STD_ERROR_HANDLE);
        _error = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    }
}

// rtypes::standard_stream
void standard_stream::_clearDevice()
{
    // (as a convinience) see if the device
    // is a console screen buffer; if so,
    // clear the screen

}
bool standard_stream::_inDevice() const
{
    return false;
}
void standard_stream::_outDevice()
{
    const char* pbuffer;
    while ( !_bufOut.is_empty() )
    {
        dword length = 0;
        while (pbuffer[length] != '\n')
            ++length;

    }
    _bufOut.clear();
}
