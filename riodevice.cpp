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

// rtypes::io_device
io_device::io_device()
{
    _input = NULL;
    _output = NULL;
}
io_device::~io_device()
{
    while (_input != NULL)
    {
        if (--_input->_reference <= 0)
            delete _input;
        if ( !_redirInput.is_empty() )
            _input = _redirInput.pop();
        else
            _input = NULL;
    }
    while (_output != NULL)
    {
        if (--_output->_reference <= 0)
            delete _output;
        if ( !_redirOutput.is_empty() )
            _output = _redirOutput.pop();
        else
            _output = NULL;
    }
}
bool io_device::open(const char* deviceID)
{
    _openEvent(deviceID,all_access);
    return _input!=NULL || _output!=NULL;
}
bool io_device::open_input(const char* deviceID)
{
    _openEvent(deviceID,read_access);
    return _input!=NULL;
}
bool io_device::open_output(const char* deviceID)
{
    _openEvent(deviceID,write_access);
    return _output!=NULL;
}
void io_device::redirect(const io_device& device)
{
    if (device._input != NULL)
    {
        if (_input != NULL) // cache the last input reference in the stack
            _redirInput.push(_input);
        _input = device._input;
        ++_input->_reference;
    }
    if (device._output != NULL)
    {
        if (_output != NULL) // cache the last output reference in the stack
            _redirOutput.push(_output);
        _output = device._output;
        ++_output->_reference;
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
        if (--_input->_reference <= 0)
            delete _input;
        _input = _redirInput.pop();
        success = true;
    }
    if ( !_redirOutput.is_empty() )
    {
        if (--_output->_reference <= 0)
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
        if (--_input->_reference <= 0)
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
        if (--_output->_reference <= 0)
            delete _output;
        _output = _redirOutput.pop();
        return true;
    }
    return false;
}
void io_device::close()
{
    while (_input != NULL)
    {
        if (--_input->_reference <= 0)
            delete _input;
        if ( !_redirInput.is_empty() )
            _input = _redirInput.pop();
        else
            _input = NULL;
    }
    while (_output != NULL)
    {
        if (--_output->_reference <= 0)
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
    while (_input != NULL)
    {
        if (--_input->_reference <= 0)
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
    while (_output != NULL)
    {
        if (--_output->_reference <= 0)
            delete _output;
        if ( !_redirOutput.is_empty() )
            _output = _redirOutput.pop();
        else
            _output = NULL;
    }
    _closeEvent(write_access);
}
