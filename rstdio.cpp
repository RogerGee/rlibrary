/* rstdio.cpp
 *  Compile target framework flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */
#include "rstdio.h"
#include "rutility.h"

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
    }
    return *this;
}
void standard_device::write_error(const char* stringBuffer)
{
    size_type t = rutil_strlen(stringBuffer);
    _writeBuffer(_error,stringBuffer,t);
}
void standard_device::redirect(const standard_device& device)
{
    // call the base class version
    static_cast<io_device*>(this)->redirect(device);
    // cache the last error reference in the stack; only perform
    // operation if there was a single valid context among input,
    // output and error
    if (_getValidContext()!=NULL && _error!=NULL)
        _redirError.push(_error);
    ++_ResourceRef(_error = device._error);
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
    bool success = static_cast<io_device*>(this)->unredirect(); // call base-class member function
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
void standard_device::_readAll(generic_string&) const
{
    // read a sizable chunk; the user can always use
    // a stream (or try again) to see if any output
    // remains
    
}
void standard_device::_closeEvent(io_access_flag flag)
{
    // if the user called close(), which shuts down
    // all IO contexts, then shutdown the error context
    // as well
    if (flag == all_access)
        close_error();
}

// rtypes::standard_stream_device
standard_stream_device::standard_stream_device()
    : _okind(out) // use standard output by default
{
}
standard_stream_device::standard_stream_device(standard_device& device)
    : stream_device<standard_device>(device), _okind(out)
{
}
void standard_stream_device::_clearDevice()
{
    _device->clear_screen();
}
bool standard_stream_device::_openDevice(const char*)
{
    return _device->open();
}
void standard_stream_device::_closeDevice()
{
    _device->close();
}

// rtypes::standard_stream
standard_stream::standard_stream()
{
}
standard_stream::standard_stream(standard_device& device)
    : standard_stream_device(device)
{
}
standard_stream::~standard_stream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    if ( !_bufOut.is_empty() )
        flush_output();
}

// rtypes::standard_binary_stream
standard_binary_stream::standard_binary_stream()
{
}
standard_binary_stream::standard_binary_stream(standard_device& device)
    : standard_stream_device(device)
{
}
standard_binary_stream::~standard_binary_stream()
{
    // complete the stream_base behavior by calling a virtual
    // member function that stream_base cannot
    if ( !_bufOut.is_empty() )
        flush_output();
}

// rstream overloads for standard_stream

rstream& rtypes::operator <<(standard_stream& stream,standard_stream_output_kind kind)
{
    stream.set_output_mode(kind);
    return stream;
}
rbinstream& rtypes::operator <<(standard_binary_stream& stream,standard_stream_output_kind kind)
{
    stream.set_output_mode(kind);
    return stream;
}

// define and setup global stream objects

static byte _stdDevice[sizeof(standard_device)];
static byte _stdConsole[sizeof(standard_stream)];
static byte _errConsole[sizeof(standard_stream)];
static standard_device& stdDevice = reinterpret_cast<standard_device&>(_stdDevice);
standard_stream& rtypes::stdConsole = reinterpret_cast<standard_stream&>(_stdConsole);
standard_stream& rtypes::errConsole = reinterpret_cast<standard_stream&>(_errConsole);

/*static*/ int __standard_console_init__::_reference = 0;
__standard_console_init__::__standard_console_init__()
{
    if (_reference++ == 0)
    {
        // explicitly invoke constructors
        new (_stdDevice) standard_device();
        new (_stdConsole) standard_stream(stdDevice);
        new (_errConsole) standard_stream(stdDevice);
        errConsole.set_output_mode(err);
    }
}
__standard_console_init__::~__standard_console_init__()
{
    if (--_reference == 0)
    {
        // explicitly invoke destructors
        stdConsole.~standard_stream();
        errConsole.~standard_stream();
        stdDevice.~standard_device();
    }
}
