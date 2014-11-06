// rfile_win32.cpp - implements rlibrary/rfile using the Windows API
#include <Windows.h>

// rtypes::file
offset_type file::get_file_pointer() const
{
}
offset_type file::get_file_size() const
{
}
bool file::eof() const
{
}
bool file::set_file_pointer(offset_type pos)
{
    io_resource* handle = _getValidContext();
    LONG low, high;
    low = pos & 0xffffffff;
    high = pos >> 32;
    if (handle != NULL)
        return SetFilePointer(handle->interpret_as<HANDLE>(),low,&high,FILE_BEGIN) != INVALID_SET_FILE_POINTER;
    throw object_not_initialized_error();
}
bool file::seek_file_pointer(offset_seek_type posDif)
{
    io_resource* handle = _getValidContext();
    LONG low, high;
    low = pos & 0xffffffff;
    high = pos >> 32;
    if (handle != NULL)
        return SetFilePointer(handle->interpret_as<HANDLE>(),low,&high,FILE_CURRENT) != INVALID_SET_FILE_POINTER;
    throw object_not_initialized_error();
}
bool file::file_truncate()
{
}
bool file::resize(offset_type size)
{
}
void file::_openEvent(const char* deviceID,io_access_flag accessKind,io_resource** pinput,io_resource** poutput,void** arguments,uint32 count)
{
    bool doesAppend;
    HANDLE hFile;
    DWORD dwDesiredAccess;
    DWORD dwShareMode;
    DWORD dwCreateDisposition;
    DWORD dwFlags;
    // deduce access mode
    dwDesiredAccess = 0;
    if (accessKind & read_access)
        dwDesiredAccess |= GENERIC_READ;
    if (accessKind & write_access)
        dwDesiredAccess |= GENERIC_WRITE;
    // use default share mode
    dwShareMode = FILE_SHARE_NONE;
    // deduce creation disposition
    if (count>=1 && arguments!=NULL)
    {
        dwCreateDisposition = 0;
        switch (*reinterpret_cast<file_open_mode*>(arguments[0]))
        {
        case file_create_exclusively:
            dwCreateDisposition = CREATE_NEW;
            break;
        case file_create_always:
            dwCreateDisposition = CREATE_ALWAYS;
            break;
        case file_open_always:
            dwCreateDisposition = OPEN_ALWAYS;
            break;
        case file_open_append:
            doesAppend = true;
            dwCreateDisposition = OPEN_ALWAYS;
            break;
        case file_open_existing:
            dwCreateDisposition = OPEN_EXISTING;
            break;
        case file_append_existing:
            doesAppend = true;
            dwCreateDisposition = OPEN_EXISTING;
            break;
        case file_truncate_existing:
            dwCreateDisposition = TRUNCATE_EXISTING;
            break;
        default:
            dwCreateDisposition = OPEN_ALWAYS;
            break;
        }
    }
    else
        // default
        dwCreateDisposition = OPEN_ALWAYS;
    // use default flags
    dwFlags = FILE_ATTRIBUTE_NORMAL;
    // attempt to create the file
    hFile = ::CreateFile(deviceID,dwDesiredAccess,dwShareMode,NULL,dwCreateDisposition,dwFlags,NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        if (doesAppend)
        {
            // set file pointer to end for append operation
            SetFilePointer(hFile,0,NULL,FILE_END);
        }
        if (accessKind & read_access)
        {
            *pinput = new io_resource;
            (*pinput)->assign(hFile);
        }
        if (accessKind & write_access)
        {
            *poutput = new io_resource(*pinput == NULL);
            (*poutput)->assign(hFile);
        }
    }
}

// rtypes::file_stream
bool file_stream::_inDevice() const
{
    char buffer[4096];
    try {
        _device->set_file_pointer(_ideviceIter);
    } catch(object_not_initialized_error&) {}
    _device->read(buffer,4096);
    // drop any \r octets from the input stream
    size_type i, cnt;
    i = 0;
    cnt = _device->get_last_byte_count();
    while (i < cnt) {
        const char* pbuf = buffer+i;
        size_type len = 0;
        while (i<cnt && pbuf[len]!='\r')
            ++len, ++i;
        _bufIn.push_range(pbuf,len);
        ++i;
    }
    // return success if at least some bytes where read
    return cnt > 0;
}
void file_stream::_outDevice()
{
    // translate \n to \r\n if no leading \r is found
    size_type iter = 0;
    const char* pbuffer = &_bufOut.peek();
    try {
        _device->set_file_pointer(_odeviceIter);
    } catch (object_not_initialized_error&) {}
    while (true) {
        size_type length = 0;
        while (iter<_bufOut.size() && pbuffer[length]!='\n' && (length==0 || pbuffer[length-1]!='\r'))
            ++length, ++iter;
        _device->write(pbuffer,length);
        if (iter >= _bufOut.size())
            break;
        _device->write("\r\n",2);
        pbuffer += length+1;
        ++iter;
    }
    _bufOut.clear();
}

// rtypes::file_binary_stream
bool file_binary_stream::_inDevice() const
{
    char buffer[4096];
    try {
        _device->set_file_pointer(_ideviceIter);
    } catch (object_not_initialized_error&) {}
    _device->read(buffer,4096);
    if (_device->get_last_operation_status() == success_read)
    {
        _bufIn.push_range(buffer,_device->get_last_byte_count());
        _ideviceIter += _device->get_last_byte_count();
        return true;
    }
    return false;
}
void file_binary_stream::_outDevice()
{
    try {
        _device->set_file_pointer(_odeviceIter);
    } catch (object_not_initialized_error&) {}
    _device->write(&_bufOut.peek(),_bufOut.size());
    _odeviceIter += _bufOut.size();
    _bufOut.clear();
}
