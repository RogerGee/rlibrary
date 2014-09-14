/* riodevice_win32.cpp - implements riodevice using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include Windows API headers
#include <Windows.h>

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

// rtypes::io_stream
bool io_stream::_inDevice() const
{
    if (_device != NULL)
    {
        /* An rstream uses \n only to represent end lines; any \r octet is
         * dropped from the input stream. This way, blocks can be read in 
         * efficiently in case the underlying device has a lot of data.
         */
        uint32 i;
        uint32 cnt;
        char buffer[4096];
        // read in a buffer
        _device->read(buffer,4096);
        cnt = _device->get_last_byte_count();
        i = 0;
        while (i < cnt)
        {
            const char* pbuf = buffer+i;
            uint32 len = 0;
            while (i<cnt && pbuf[len]!='\r')
                ++len, ++i;
            _bufIn.push_range(pbuf,len);
            ++i;
        }
        // return success if at least some bytes where read
        return cnt > 0;
    }
    return false;
}
void io_stream::_outDevice()
{
    if (_device != NULL)
    {
        /* On Windows, the \r\n endline encoding is commonly used on many
         * devices. Therefore, all \n characters are translated into the
         * sequence \r\n given that no preceding \r was found.
         */
        uint32 iter = 0;
        const char* pbuffer = &_bufOut.peek();
        while (true)
        {
            uint32 length = 0;
            while (iter<_bufOut.size() && pbuffer[length]!='\n' && (length==0 || pbuffer[length-1]!='\r'))
                ++length, ++iter;
            _device->write(pbuffer,length);
            if (iter >= _bufOut.size())
                break;
            _device->write("\r\n",2);
            pbuffer += length+1;
            ++iter;
        }
        _bufOut.clear();
    }
}

// rtypes::binary_io_stream
bool binary_io_stream::_inDevice() const
{
    if (_device != NULL)
    {
        char buffer[4096];
        _device->read(buffer,4096);
        if (_device->get_last_operation_status() == success_read)
        {
            _bufIn.push_range(buffer,_device->get_last_byte_count());
            return true;
        }
        return false;
    }
}
void binary_io_stream::_outDevice()
{
    if (_device != NULL)
    {
        _device->write(&_bufOut.peek(),_bufOut.size());
        _bufOut.clear();
    }
}
