/* riodevice.cpp
 *  Compile target platform flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */
#include "riodevice.h"
#include "rlasterr.h"

// define target-independent constants

// create invalid IO resource constant;
// the default constructor will initialize
// this to the proper value
static const rtypes::io_resource INVALID_RESOURCE;

// define target-specific code
//  -> Gets: 'using namespace rtypes'

#if defined(RLIBRARY_BUILD_POSIX)
#include "riodevice_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "riodevice_win32.cpp"
#else
using namespace rtypes;
#endif

// define target-independent code

// rtypes::io_resource
io_resource::io_resource(void* defaultValue)
    : _MyBase(defaultValue)
{
    _reference = 0;
}

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
io_device& io_device::operator =(const io_device& device)
{
    if (this != &device)
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
    return *this;
}
str io_device::read(dword bytesToRead)
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
bool io_device::open(const char* deviceID)
{
    if (_input!=NULL || _output!=NULL)
        return false; // must close both first
    _openEvent(deviceID,all_access); // might open both or just one
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
    _openEvent(deviceID,read_access); // should only open input
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
    _openEvent(deviceID,write_access);
    if (_output != NULL) // should only open output
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
void io_device::redirect(const io_device& device)
{
    bool a, b;
    a = device._input != NULL;
    b = device._output != NULL;
    if (a)
    {
        // cache the last input reference in the stack
        if (_input!=NULL || _output!=NULL)
            _redirInput.push(_input);
        _input = device._input;
        ++_input->_reference;
        if (!b) // if one context changes, the other must as well
            _redirOutput.push(NULL);
    }
    if (b)
    {
        if (_input!=NULL || _output!=NULL) // cache the last output reference in the stack
            _redirOutput.push(_output);
        _output = device._output;
        ++_output->_reference;
        if (!a) // if one context changes, the other must as well
            _redirInput.push(NULL);
    }
}
void io_device::redirect_input(const io_device& device)
{
    if (device._input != NULL)
    {
        if (_input != NULL) // cache the last input reference in the stack
            _redirInput.push(_input);
        _input = device._input;
        ++_input->_reference;
    }
}
void io_device::redirect_output(const io_device& device)
{
    if (device._output != NULL)
    {
        if (_output != NULL) // cache the last output reference in the stack
            _redirOutput.push(_output);
        _output = device._output;
        ++_output->_reference;
    }
}
bool io_device::unredirect()
{
    bool success = false;
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
/* static */ int& io_device::_ResourceRef(io_resource* pres)
{
    return pres->_reference;
}
