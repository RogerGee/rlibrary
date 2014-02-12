// rstringstream.cpp
#include "rstringstream.h"
using namespace rtypes;

rstringstream::rstringstream()
{
    // allow default construction
}
rstringstream::rstringstream(const char* initialValue)
{
    // construct with default value (enforced by the behavior of device ID open)
    open(initialValue);
}
rstringstream::rstringstream(generic_string& strDevice)
    : stream_device<str,generic_string>(strDevice)
{

}
rstringstream::~rstringstream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    flush_output();
}
void rstringstream::_clearDevice()
{
    _device->clear();
}
bool rstringstream::_openDevice(const char* deviceID)
{
    *_device = deviceID;
    _odeviceIter = _device->length(); // open for appending
    return true; // always succeeds
}
void rstringstream::_closeDevice()
{
    // a string doesn't have any close action
}
bool rstringstream::_inDevice() const
{
    // place all available bytes from the string into the buffer
    if (_ideviceIter < _device->length())
    {
        while (_ideviceIter < _device->length())
            _bufIn.push( _device->operator[](_ideviceIter++) );
        return true; // data was put into the stream
    }
    return false; // data was not put into the stream
}
void rstringstream::_outDevice()
{
    const char* data = &_bufOut.peek();
    for (dword i = 0;i<_bufOut.size();i++,_odeviceIter++)
        if (_odeviceIter < _device->length()) // replace existing character
            _device->operator[](_odeviceIter) = data[i];
        else // grow the string
            _device->push_back(data[i]);
    _bufOut.pop_range( _bufOut.size() );
}

const_rstringstream::const_rstringstream()
{
    // allow for default construction
}
const_rstringstream::const_rstringstream(const char* initialValue)
{
    // construct with default value (enforced by the behavior of device ID open)
    open(initialValue);
}
const_rstringstream::const_rstringstream(const generic_string& strDevice)
    : const_stream_device<str,generic_string>(strDevice)
{

}
bool const_rstringstream::_openDevice(generic_string* device,const char* deviceID)
{
    *device = deviceID;
    return true; // always succeeds
}
void const_rstringstream::_closeDevice()
{
    // a string doesn't have any close action
}
bool const_rstringstream::_inDevice() const
{
    // place all available bytes from the string into the buffer
    if (_ideviceIter < _device->length())
    {
        while (_ideviceIter < _device->length())
            _bufIn.push( _device->operator[](_ideviceIter++) );
        return true; // data was put into the stream
    }
    return false; // data was not put into the stream
}

rbinstringstream::rbinstringstream()
{
    // allow default construction
}
rbinstringstream::rbinstringstream(const char* initialValue)
{
    // construct with default value (enforced by the behavior of device ID open)
    open(initialValue);
}
rbinstringstream::rbinstringstream(generic_string& strDevice)
    : stream_device<str,generic_string>(strDevice)
{

}
rbinstringstream::rbinstringstream(endianness endianFlag)
    : rbinstream(endianFlag)
{
}
rbinstringstream::~rbinstringstream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    flush_output();
}
void rbinstringstream::_clearDevice()
{
    _device->clear();
}
bool rbinstringstream::_openDevice(const char* deviceID)
{
    *_device = deviceID;
    _odeviceIter = _device->length();
    return true; // always succeeds
}
void rbinstringstream::_closeDevice()
{
    // a string doesn't have any close action
}
bool rbinstringstream::_inDevice() const
{
    // place all available bytes from the string into the buffer
    if (_ideviceIter < _device->length())
    {
        while (_ideviceIter < _device->length())
            _bufIn.push( _device->operator[](_ideviceIter++) );
        return true; // data was put into the stream
    }
    return false; // data was not put into the stream
}
void rbinstringstream::_outDevice()
{
    const char* data = &_bufOut.peek();
    for (dword i = 0;i<_bufOut.size();i++,_odeviceIter++)
        if (_odeviceIter < _device->length()) // replace existing character
            _device->operator[](_odeviceIter) = data[i];
        else // grow the string
            _device->push_back(data[i]);
    _bufOut.pop_range( _bufOut.size() );
}

const_rbinstringstream::const_rbinstringstream()
{
    // allow for default construction
}
const_rbinstringstream::const_rbinstringstream(const char* initialValue)
{
    // construct with default value (enforced by the behavior of device ID open)
    open(initialValue);
}
const_rbinstringstream::const_rbinstringstream(const generic_string& strDevice)
    : const_stream_device<str,generic_string>(strDevice)
{

}
const_rbinstringstream::const_rbinstringstream(endianness endianFlag)
    : rbinstream(endianFlag)
{
}
bool const_rbinstringstream::_openDevice(generic_string* device,const char* deviceID)
{
    *device = deviceID;
    return true; // always succeeds
}
void const_rbinstringstream::_closeDevice()
{
    // a string doesn't have any close action
}
bool const_rbinstringstream::_inDevice() const
{
    // place all available bytes from the string into the buffer
    if (_ideviceIter < _device->length())
    {
        while (_ideviceIter < _device->length())
            _bufIn.push( _device->operator[](_ideviceIter++) );
        return true; // data was put into the stream
    }
    return false; // data was not put into the stream
}
