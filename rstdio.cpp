/* rstdio.cpp
 *  Compile target platform flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */
#include "rstdio.h"

// define target-specific code
//  -> Gets: 'using namespace rtypes'

#if defined (RLIBRARY_BUILD_POSIX)
#include "rstdio_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "rstdio_win32.cpp"
#else
using namespace rtypes;
#endif

// define target-independent code

// rtypes::standard_device
standard_device::standard_device()
    : _error(NULL)
{
    // open all IO contexts by default
    open();
}
standard_device::standard_device(io_access_flag desiredAccess)
    : _error(NULL)
{
    if (desiredAccess == all_access)
        open();
    else if (desiredAccess == read_access)
        open_input();
    else if (desiredAccess == write_access)
        open_output();
}
standard_device::standard_device(const standard_device& device)
    : io_device(device) // invoke base-class copy operation
{
    _error = device._error;
    if (_error != NULL)
        ++_ResourceRef(_error);
    // have to use derived version of is_valid_context
    _lastOp = is_valid_context() ? no_operation : no_device;
}
standard_device::~standard_device()
{
    while (_error!=NULL || !_redirError.is_empty())
    {
        if (_error!=NULL && --_ResourceRef(_error)<=0)
            delete _error;
        if ( !_redirError.is_empty() )
            _error = _redirError.pop();
        else
            _error = NULL;
    }
}
standard_device& standard_device::operator =(const standard_device& device)
{
    if (this != &device)
    {
        // invoke base-class assignment operation
        _assign(device);
        // copy error
        if (_error!=NULL && --_ResourceRef(_error)<=0)
            delete _error;
        _error = device._error;
        if (_error != NULL)
            ++_ResourceRef(_error);
        // have to use derived version of is_valid_context
        _lastOp = is_valid_context() ? no_operation : no_device;
    }
    return *this;
}
void standard_device::redirect(const standard_device& device)
{
    bool a, b, c;
    a = device._input != NULL;
    b = device._output != NULL;
    c = device._error != NULL;
    if (a)
    {
        // cache the last input reference in the stack
        if (_input!=NULL || _output!=NULL || _error!=NULL)
            _redirInput.push(_input);
        _input = device._input;
        ++_ResourceRef(_input);
        // if a single context changes, the others must as well
        if (!b)
            _redirOutput.push(NULL);
        if (!c)
            _redirError.push(NULL);
    }
    if (b)
    {
        // cache the last output reference in the stack
        if (_input!=NULL || _output!=NULL || _error!=NULL)
            _redirOutput.push(_output);
        _output = device._output;
        ++_ResourceRef(_output);
        // if a single context changes, the others must as well
        if (!a)
            _redirInput.push(NULL);
        if (!c)
            _redirError.push(NULL);
    }
    if (c)
    {
        // cache the last error reference in the stack
        if (_input!=NULL || _output!=NULL || _error!=NULL)
            _redirError.push(_error);
        _error = device._error;
        ++_ResourceRef(_error);
        // if a single context changes, the others must as well
        if (!a)
            _redirInput.push(NULL);
        if (!b)
            _redirOutput.push(NULL);
    }
}
void standard_device::redirect_error(const standard_device& device)
{
    if (device._error != NULL)
    {
        if (_error != NULL) // cache the last error reference in the stack
            _redirError.push(_error);
        _error = device._error;
        ++_ResourceRef(_error);
    }
}
bool standard_device::unredirect()
{
    // this is a non-virtual override for this derivation; it un-redirects
    // the standard error context as well as the input and output contexts
    bool success = false;
    if ( !_redirInput.is_empty() )
    {
        if (_input!=NULL && --_ResourceRef(_input)<=0)
            delete _input;
        _input = _redirInput.pop();
        success = true;
    }
    if ( !_redirOutput.is_empty() )
    {
        if (_output!=NULL && --_ResourceRef(_output)<=0)
            delete _output;
        _output = _redirOutput.pop();
        success = true;
    }
    if ( !_redirError.is_empty() )
    {
        if (_error!=NULL && --_ResourceRef(_error)<=0)
            delete _error;
        _error = _redirError.pop();
        success = true;
    }
    return success;
}
bool standard_device::unredirect_error()
{
    if ( !_redirError.is_empty() )
    {
        if (_error!=NULL && --_ResourceRef(_error)<=0)
            delete _error;
        _error = _redirError.pop();
        return true;
    }
    return false;
}
void standard_device::close_error()
{
    while (_error!=NULL || !_redirError.is_empty())
    {
        if (_error!=NULL && --_ResourceRef(_error)<=0)
            delete _error;
        if ( !_redirError.is_empty() )
            _error = _redirError.pop();
        else
            _error = NULL;
    }
    // (no close event is called for error device extension)
}
bool standard_device::is_redirected_error() const
{
    return _redirError.size() > 0;
}
bool standard_device::is_valid_error() const
{
    return _error != NULL;
}
void standard_device::_readAll(generic_string&)
{
}
void standard_device::_closeEvent(io_access_flag)
{
}
