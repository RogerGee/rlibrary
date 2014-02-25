/* riodevice_win32.cpp - implements riodevice using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include Windows API headers
#include <Windows.h>
using namespace rtypes;

// rtypes::io_resource
io_resource::io_resource(bool closable)
    : _MyBase(INVALID_HANDLE_VALUE)
{
    _reference = 1; // set default reference to 1 use
    _closable = closable;
}
io_resource::~io_resource()
{
    if (*this!=INVALID_RESOURCE && _closable)
    {
        if ( ::CloseHandle( interpret_as<HANDLE>() )==0 )
            rlib_last_error::switch_throw();
        else
            this->assign(INVALID_HANDLE_VALUE);
    }
}

// rtypes::io_device
void io_device::_readBuffer(void* buffer,uint32 bytesToRead) const
{
    if (_input != NULL)
    {
        DWORD bytesRead;
        if ( !::ReadFile(_input->interpret_as<HANDLE>(),buffer,bytesToRead,&bytesRead,NULL) )
        {
            _lastOp = bad_read;
            _byteCount = 0;
        }
        else if (bytesRead == 0)
        {
            _lastOp = no_input;
            _byteCount = 0;
        }
        else
        {
            _lastOp = success_read;
            _byteCount = bytesRead;
        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
void io_device::_readBuffer(const io_resource* context,void* buffer,uint32 bytesToRead) const
{
    if (context != NULL)
    {
        DWORD bytesRead;
        if ( !::ReadFile(context->interpret_as<HANDLE>(),buffer,bytesToRead,&bytesRead,NULL) )
        {
            _lastOp = bad_read;
            _byteCount = 0;
        }
        else if (bytesRead == 0)
        {
            _lastOp = no_input;
            _byteCount = 0;
        }
        else
        {
            _lastOp = success_read;
            _byteCount = bytesRead;
        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
void io_device::_writeBuffer(const void* buffer,uint32 length)
{
    if (_output != NULL)
    {
        DWORD bytesWrote;
        if ( !::WriteFile(_output->interpret_as<HANDLE>(),buffer,length,&bytesWrote,NULL) )
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
void io_device::_writeBuffer(const io_resource* context,const void* buffer,uint32 length)
{
    if (context != NULL)
    {
        DWORD bytesWrote;
        if ( !::WriteFile(context->interpret_as<HANDLE>(),buffer,length,&bytesWrote,NULL) )
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
