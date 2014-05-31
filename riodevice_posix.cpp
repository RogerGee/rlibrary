/* riodevice_posix.cpp - implements riodevice using POSIX
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include POSIX and other system headers
#include <unistd.h>
using namespace rtypes;

// rtypes::io_resource
io_resource::io_resource(bool closable)
    : _MyBase(-1)
{
    _reference = 1; // set default reference to 1 use
    _closable = closable;
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
void io_device::_readBuffer(void* buffer,size_type bytesToRead) const
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
            _byteCount = uint32(bytesRead);
        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
void io_device::_readBuffer(const io_resource* context,void* buffer,size_type bytesToRead) const
{
    if (context != NULL)
    {
        ssize_t bytesRead;
        bytesRead = ::read(context->interpret_as<int>(),buffer,bytesToRead);
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
            _byteCount = uint32(bytesRead);
        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
void io_device::_writeBuffer(const void* buffer,size_type length)
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
            _byteCount = uint32(bytesWrote);
        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
void io_device::_writeBuffer(const io_resource* context,const void* buffer,size_type length)
{
    if (context != NULL)
    {
        ssize_t bytesWrote;
        bytesWrote = ::write(context->interpret_as<int>(),buffer,length);
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
            _byteCount = uint32(bytesWrote);
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
        char buffer[4096];
        _device->read(buffer,4096);
        if (_device->get_last_operation_status() == success_read)
        {
            _bufIn.push_range(buffer,_device->get_last_byte_count());
            return true;
        }
    }
    return false;
}
void io_stream::_outDevice()
{
    if (_device != NULL)
    {
        _device->write(&_bufOut.peek(),_bufOut.size());
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
    }
    return false;
}
void binary_io_stream::_outDevice()
{
    if (_device != NULL)
    {
        _device->write(&_bufOut.peek(),_bufOut.size());
        _bufOut.clear();
    }
}
