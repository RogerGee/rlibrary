/* rstdio_posix.cpp - implements rstdio using POSIX
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include system headers
#include <unistd.h>
using namespace rtypes;

// rtypes::standard_device
bool standard_device::open_error(const char*)
{
    _error = new io_resource(false);
    _error->assign(STDERR_FILENO);
    return true;
}
bool standard_device::clear_screen()
{
    // (as a convinience) see if the device
    // is a terminal and send it the ASCII
    // escape code for clearing the screen
    io_resource* output = _getOutputContext();
    if (::isatty( output->interpret_as<int>() ))
    {
        
        return true;
    }
    return false;
}
void standard_device::_openEvent(const char*,io_access_flag kind,io_resource** pinput,io_resource** poutput,void**,dword)
{
    if (kind & read_access)
    {
        *pinput = new io_resource(false);
        (*pinput)->assign(STDIN_FILENO);
    }
    if (kind & write_access)
    {
        *poutput = new io_resource(false);
        (*poutput)->assign(STDOUT_FILENO);
    }
    if (kind == all_access)
    {
        _error = new io_resource(false);
        _error->assign(STDERR_FILENO);
    }
}

// rtypes::standard_stream
bool standard_stream::_inDevice() const
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
void standard_stream::_outDevice()
{
    if (_okind == out)
        _device->write(&_bufOut.peek(),_bufOut.size());
    else
        _device->write_error(&_bufOut.peek(),_bufOut.size());
    _bufOut.clear();
}

// rtypes::standard_binary_stream
bool standard_binary_stream::_inDevice() const
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
void standard_binary_stream::_outDevice()
{
    if (_okind == out)
        _device->write(&_bufOut.peek(),_bufOut.size());
    else
        _device->write_error(&_bufOut.peek(),_bufOut.size());
    _bufOut.clear();
}
