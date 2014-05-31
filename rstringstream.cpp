// rstringstream.cpp
#include "rstringstream.h"
using namespace rtypes;

stringstream_io::stringstream_io()
    : rstream(false)
{ // don't buffer local characters by default
}
bool stringstream_io::_inDeviceImpl(const generic_string* device) const
{
    // place all available bytes from the string into the buffer
    if (device!=NULL && _ideviceIter<device->length())
    {
        while (_ideviceIter < device->length())
            _bufIn.push( device->operator[](_ideviceIter++) );
        return true; // data was put into the stream
    }
    return false; // data was not put into the stream
}
void stringstream_io::_outDeviceImpl(generic_string* device)
{
    if (device != NULL)
    {
        const char* data = &_bufOut.peek();
        for (uint32 i = 0;i<_bufOut.size();i++,_odeviceIter++)
            if (_odeviceIter < device->length()) // replace existing character
                device->operator[](_odeviceIter) = data[i];
            else // grow the string
                device->push_back(data[i]);
        _bufOut.pop_range( _bufOut.size() );
    }
}

binstringstream_io::binstringstream_io()
    : rbinstream(false)
{ // don't buffer local bytes by default
}
binstringstream_io::binstringstream_io(endianness e)
    : rbinstream(e,false)
{ // set endianness flag; don't buffer local bytes by default
}
bool binstringstream_io::_inDeviceImpl(const generic_string* device) const
{
    // place all available bytes from the string into the buffer
    if (_ideviceIter < device->length())
    {
        while (_ideviceIter < device->length())
            _bufIn.push( device->operator[](_ideviceIter++) );
        return true; // data was put into the stream
    }
    return false; // data was not put into the stream
}
void binstringstream_io::_outDeviceImpl(generic_string* device)
{
    const char* data = &_bufOut.peek();
    for (uint32 i = 0;i<_bufOut.size();i++,_odeviceIter++)
        if (_odeviceIter < device->length()) // replace existing character
            device->operator[](_odeviceIter) = data[i];
        else // grow the string
            device->push_back(data[i]);
    _bufOut.pop_range( _bufOut.size() );
}


void string_stream_device::_clearDevice()
{
    _device->clear();
}
bool string_stream_device::_openDevice(const char* deviceID)
{
    *_device = deviceID;
    _odeviceIter = _device->length(); // open for appending
    return true; // always succeeds
}
void string_stream_device::_closeDevice()
{ // a string doesn't have any close action
}

void generic_string_stream_device::_clearDevice()
{
    if (_device != NULL)
        _device->clear();
}
bool generic_string_stream_device::_openDevice(const char* deviceID)
{
    if (_device != NULL)
    {
        *_device = deviceID;
        _odeviceIter = _device->length(); // open for appending
        return true; // always succeeds
    }
    return false;
}
void generic_string_stream_device::_closeDevice()
{ // a string doesn't have any close action
}

bool const_string_stream_device::_openDevice(generic_string* device,const char* deviceID)
{
    *device = deviceID;
    return true; // always succeeds
}
void const_string_stream_device::_closeDevice()
{ // a string doesn't have any close action
}


stringstream::stringstream()
{ // allow default construction
}
stringstream::stringstream(const char* initialValue)
{
    // construct with default value (enforced by the behavior of device ID open)
    open(initialValue);
}
stringstream::stringstream(generic_string& strDevice)
    : string_stream_device(strDevice)
{
}
stringstream::~stringstream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    _outDevice();
}

ref_stringstream::ref_stringstream()
{ // allow for default construction
}
ref_stringstream::ref_stringstream(generic_string& strDevice)
    : generic_string_stream_device(strDevice)
{
}
ref_stringstream::~ref_stringstream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    _outDevice();
}

const_stringstream::const_stringstream()
{ // allow for default construction
}
const_stringstream::const_stringstream(const generic_string& strDevice)
    : const_string_stream_device(strDevice)
{
}

binstringstream::binstringstream()
{ // allow default construction
}
binstringstream::binstringstream(const char* initialValue)
{
    // construct with default value (enforced by the behavior of device ID open)
    open(initialValue);
}
binstringstream::binstringstream(generic_string& strDevice)
    : string_stream_device(strDevice)
{
}
binstringstream::binstringstream(endianness endianFlag)
    : binstringstream_io(endianFlag)
{
}
binstringstream::~binstringstream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    _outDevice();
}

ref_binstringstream::ref_binstringstream()
{ // allow for default construction
}
ref_binstringstream::ref_binstringstream(generic_string& strDevice)
    : generic_string_stream_device(strDevice)
{
}
ref_binstringstream::ref_binstringstream(endianness endianFlag)
    : binstringstream_io(endianFlag)
{
}
ref_binstringstream::~ref_binstringstream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    _outDevice();
}

const_binstringstream::const_binstringstream()
{ // allow for default construction
}
const_binstringstream::const_binstringstream(const generic_string& strDevice)
    : const_string_stream_device(strDevice)
{
}
const_binstringstream::const_binstringstream(endianness endianFlag)
    : binstringstream_io(endianFlag)
{
}
