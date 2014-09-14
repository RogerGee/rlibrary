/* riodevice.cpp
 *  Compile target framework flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */
#include "riodevice.h"
#include "rlasterr.h"
#include "rutility.h"
using namespace rtypes;

// define target-independent constants

// create invalid IO resource constant;
// the default constructor will initialize
// this to the proper value
static const io_resource INVALID_RESOURCE;

// define target-specific code

#if defined(RLIBRARY_BUILD_POSIX)
#include "riodevice_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "riodevice_win32.cpp"
#endif

// define target-independent code

// rtypes::io_device
io_device::io_device()
{
    _input = NULL;
    _output = NULL;
    _lastOp = no_device;
    _byteCount = 0;
}
io_device::io_device(const io_device& device)
{
    _input = device._input;
    if (_input != NULL)
        ++_input->_reference;
    _output = device._output;
    if (_output != NULL)
        ++_output->_reference;
    _lastOp = is_valid_context() ? no_operation : no_device;
    _byteCount = 0;
}
io_device::~io_device()
{
    // (close implementation without virtual member function call)
    while (_input!=NULL || !_redirInput.is_empty())
    {
        if (_input!=NULL && --_input->_reference<=0)
            delete _input;
        if ( !_redirInput.is_empty() )
            _input = _redirInput.pop();
        else
            _input = NULL;
    }
    while (_output!=NULL || !_redirOutput.is_empty())
    {
        if (_output!=NULL && --_output->_reference<=0)
            delete _output;
        if ( !_redirOutput.is_empty() )
            _output = _redirOutput.pop();
        else
            _output = NULL;
    }
}
str io_device::read(size_type bytesToRead) const
{
    // I provide this overload as a higher-level convinience
    str buffer;
    if (bytesToRead == 0)
        _readAll(buffer); // let the derived implementation figure out how to read all bytes from the device
    else
    {
        // allocate buffer size
        buffer.resize(bytesToRead);
        _readBuffer(&buffer[0],buffer.size());
    }
    return buffer; // (trims extra bytes in buffer)
}
void io_device::write(const char* stringBuffer)
{
    size_type t = rutil_strlen(stringBuffer);
    _writeBuffer(stringBuffer,t);
}
bool io_device::open(const char* deviceID)
{
    if (_input!=NULL || _output!=NULL)
        return false; // must close both first
    _openEvent(deviceID,all_access,&_input,&_output); // might open both or just one
    if (_input!=NULL || _output!=NULL)
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
bool io_device::open_input(const char* deviceID)
{
    if (_input != NULL)
        return false; // must close input first
    _openEvent(deviceID,read_access,&_input,NULL); // should only open input
    if (_input != NULL)
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
bool io_device::open_output(const char* deviceID)
{
    if (_output != NULL)
        return false; // must close output first
    _openEvent(deviceID,write_access,NULL,&_output);
    if (_output != NULL) // should only open output
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
void io_device::redirect(const io_device& device)
{
    if (device._input!=NULL || device._output!=NULL)
    {
        // cache the last state if at least one 
        // valid context exists
        if (_input!=NULL || _output!=NULL)
        {
            _redirInput.push(_input);
            _redirOutput.push(_output);
        }
        // assign context references from device;
        // increment the reference count
        ++(_input = device._input)->_reference;
        ++(_output = device._output)->_reference;
    }
}
void io_device::redirect_input(const io_device& device)
{
    if (device._input != NULL)
    {
        if (_input != NULL) // cache the last input reference in the stack
            _redirInput.push(_input);
        ++(_input = device._input)->_reference;
    }
}
void io_device::redirect_output(const io_device& device)
{
    if (device._output != NULL)
    {
        if (_output != NULL) // cache the last output reference in the stack
            _redirOutput.push(_output);
        ++(_output = device._output)->_reference;
    }
}
bool io_device::unredirect()
{
    bool success = false;
    // attempt to pop a context from
    // either stack; lose reference to
    // current context(s) (decrement count)
    if ( !_redirInput.is_empty() )
    {
        if (_input!=NULL && --_input->_reference<=0)
            delete _input;
        _input = _redirInput.pop();
        success = true;
    }
    if ( !_redirOutput.is_empty() )
    {
        if (_output!=NULL && --_output->_reference<=0)
            delete _output;
        _output = _redirOutput.pop();
        success = true;
    }
    return success;
}
bool io_device::unredirect_input()
{
    // attempt to pop an input context
    // from the stack; lose reference to 
    // current context (decrement count)
    if ( !_redirInput.is_empty() )
    {
        if (_input!=NULL && --_input->_reference<=0)
            delete _input;
        _input = _redirInput.pop();
        return true;
    }
    return false;
}
bool io_device::unredirect_output()
{
    // attempt to pop an output context
    // from the stack; lose reference to 
    // current context (decrement count)
    if ( !_redirOutput.is_empty() )
    {
        if (_output!=NULL && --_output->_reference<=0)
            delete _output;
        _output = _redirOutput.pop();
        return true;
    }
    return false;
}
void io_device::close()
{
    while (_input!=NULL || !_redirInput.is_empty())
    {
        if (_input!=NULL && --_input->_reference<=0)
            delete _input;
        if ( !_redirInput.is_empty() )
            _input = _redirInput.pop();
        else
            _input = NULL;
    }
    while (_output!=NULL || !_redirOutput.is_empty())
    {
        if (_output!=NULL && --_output->_reference<=0)
            delete _output;
        if ( !_redirOutput.is_empty() )
            _output = _redirOutput.pop();
        else
            _output = NULL;
    }
    _closeEvent(all_access);
}
void io_device::close_input()
{
    while (_input!=NULL || !_redirInput.is_empty())
    {
        if (_input!=NULL && --_input->_reference<=0)
            delete _input;
        if ( !_redirInput.is_empty() )
            _input = _redirInput.pop();
        else
            _input = NULL;
    }
    _closeEvent(read_access);
}
void io_device::close_output()
{
    while (_output!=NULL || !_redirOutput.is_empty())
    {
        if (_output!=NULL && --_output->_reference<=0)
            delete _output;
        if ( !_redirOutput.is_empty() )
            _output = _redirOutput.pop();
        else
            _output = NULL;
    }
    _closeEvent(write_access);
}
bool io_device::is_redirected_input() const
{
    return _redirInput.size() > 0;
}
bool io_device::is_redirected_output() const
{
    return _redirOutput.size() > 0;
}
bool io_device::is_valid_input() const
{
    return _input != NULL;
}
bool io_device::is_valid_output() const
{
    return _output != NULL;
}
io_device& io_device::_assign(const io_device& device)
{
    if (this != &device)
    {
        if (_input!=NULL && --_input->_reference<=0)
            delete _input;
        _input = device._input;
        if (_input != NULL)
            ++_input->_reference;
        if (_output!=NULL && --_output->_reference<=0)
            delete _output;
        _output = device._output;
        if (_output != NULL)
            ++_output->_reference;
        _lastOp = is_valid_context() ? no_operation : no_device;
        _byteCount = 0;
    }
    return *this;
}
io_device& io_device::_assign(io_resource* inputContext,io_resource* outputContext)
{
    if (_input!=NULL && --_input->_reference<=0)
        delete _input;
    _input = inputContext;
    if (_input != NULL)
        ++_input->_reference;
    if (_output!=NULL && --_output->_reference<=0)
        delete _output;
    _output = outputContext;
    if (_output != NULL)
        ++_output->_reference;
    _lastOp = is_valid_context() ? no_operation : no_device;
    _byteCount = 0;
    return *this;
}
const io_resource* io_device::_getValidContext() const
{
    if (_input != NULL)
        return _input;
    if (_output != NULL)
        return _output;
    return NULL;
}
io_resource* io_device::_getValidContext()
{
    if (_input != NULL)
        return _input;
    if (_output != NULL)
        return _output;
    return NULL;
}
bool io_device::_openWithArgs(const char* deviceID,void** arguments,uint32 argumentCount)
{
    if (_input!=NULL || _output!=NULL)
        return false; // must close both first
    _openEvent(deviceID,all_access,&_input,&_output,arguments,argumentCount); // might open both or just one
    if (_input!=NULL || _output!=NULL)
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
bool io_device::_openInputWithArgs(const char* deviceID,void** arguments,uint32 argumentCount)
{
    if (_input != NULL)
        return false; // must close input first
    _openEvent(deviceID,read_access,&_input,NULL,arguments,argumentCount); // should only open input
    if (_input != NULL)
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
bool io_device::_openOutputWithArgs(const char* deviceID,void** arguments,uint32 argumentCount)
{
    if (_output != NULL)
        return false; // must close output first
    _openEvent(deviceID,write_access,NULL,&_output,arguments,argumentCount); // should only open output
    if (_output != NULL) // should only open output
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
/* static */ int& io_device::_ResourceRef(io_resource* pres)
{
    return pres->_reference;
}

// rtypes::io_stream_device
io_stream_device::io_stream_device()
{ // allow default construction
}
io_stream_device::io_stream_device(io_device& device)
    : generic_stream_device<io_device>(device)
{
}
void io_stream_device::_clearDevice()
{
    // no implementation available
}
bool io_stream_device::_openDevice(const char* deviceID)
{
    return _device->open(deviceID);
}
void io_stream_device::_closeDevice()
{
    _device->close();
}

// rtypes::io_stream
io_stream::io_stream()
{ // allow default construction
}
io_stream::io_stream(io_device& device)
    : io_stream_device(device)
{
}
io_stream::~io_stream()
{
    // complete rstream behavior by performing
    // virtual function call
    if ( !_bufOut.is_empty() )
        _outDevice();
}

// rtypes::binary_io_stream
binary_io_stream::binary_io_stream()
{ // allow default construction
}
binary_io_stream::binary_io_stream(io_device& device)
    : io_stream_device(device)
{
}
binary_io_stream::binary_io_stream(endianness endianFlag)
    : rbinstream(endianFlag)
{
}
binary_io_stream::~binary_io_stream()
{
    // complete rstream behavior by performing
    // virtual function call
    if ( !_bufOut.is_empty() )
        _outDevice();
}
