/* rfile.cpp
 *  Compile target framework flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */
#include "rfile.h"
#include "rlasterr.h"

// define target-specific code
// -> gets `using namespace rtypes;'

#if defined(RLIBRARY_BUILD_POSIX)
#include "rfile_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "rfile_win32.cpp"
#else
using namespace rtypes;
#endif

// define target-independent code

// rtypes::file
file::file()
{
}
file::file(const char* fileName,file_open_mode mode)
{
    if ( !open(fileName,mode) )
        throw *rlib_last_error::get();
}
bool file::open(const char* fileName,file_open_mode mode)
{
    if (_input!=NULL || _output!=NULL)
        return false; // must close both first
    void* args[1];
    args[0] = &mode;
    _openEvent(fileName,all_access,args,1);
    if (_input!=NULL || _output!=NULL)
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
bool file::open_input(const char* fileName,file_open_mode mode)
{
    if (_input != NULL)
        return false; // must close input first
    void* args[1];
    args[0] = &mode;
    _openEvent(fileName,read_access,args,1);
    if (_input != NULL)
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
bool file::open_output(const char* fileName,file_open_mode mode)
{
    if (_output != NULL)
        return false; // must close output first
    void* args[1];
    args[0] = &mode;
    _openEvent(fileName,write_access,args,1);
    if (_output != NULL)
    {
        _lastOp = no_operation;
        return true;
    }
    return false;
}
void file::_closeEvent(io_access_flag)
{
}

// rtypes::file_stream_device
file_stream_device::file_stream_device()
{
}
file_stream_device::file_stream_device(file& device)
    : stream_device<file>(device)
{
}
void file_stream_device::_clearDevice()
{
    _device->truncate();
}
bool file_stream_device::_openDevice(const char* deviceID)
{
    return _device->open(deviceID,file_open_always);
}
void file_stream_device::_closeDevice()
{
    _device->close();
}

// rtypes::file_stream
file_stream::file_stream()
{
}
file_stream::file_stream(const char* fileName)
{
    open(fileName);
}
file_stream::file_stream(file& fileDevice)
    : file_stream_device(fileDevice)
{
}
file_stream::~file_stream()
{
    // ensure that the data will
    // be flushed to the device
    if ( !_bufOut.is_empty() )
        _outDevice();
}

// rtypes::file_binary_stream
file_binary_stream::file_binary_stream()
{
}
file_binary_stream::file_binary_stream(const char* fileName)
{
    open(fileName);
}
file_binary_stream::file_binary_stream(file& fileDevice)
    : file_stream_device(fileDevice)
{
}
file_binary_stream::~file_binary_stream()
{
    if ( !_bufOut.is_empty() )
        _outDevice();
}
