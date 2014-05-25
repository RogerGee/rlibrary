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
    open(fileName,mode);
}
file& file::operator =(const file& obj)
{
    if (this != &obj)
        _assign(obj);
    return *this;
}
bool file::open(const char* fileName,file_open_mode mode)
{
    void* args[1];
    args[0] = &mode;
    return _openWithArgs(fileName,args,1);
}
bool file::open_input(const char* fileName,file_open_mode mode)
{
    void* args[1];
    args[0] = &mode;
    return _openInputWithArgs(fileName,args,1);
}
bool file::open_output(const char* fileName,file_open_mode mode)
{
    void* args[1];
    args[0] = &mode;
    return _openOutputWithArgs(fileName,args,1);
}
void file::_readAll(generic_string& buffer) const
{
    buffer.resize( get_file_size() );
    read(buffer);
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
