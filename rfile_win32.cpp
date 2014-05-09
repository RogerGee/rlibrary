// rfile_win32.cpp - implements rlibrary/rfile using the Windows API
#include <Windows.h>
using namespace rtypes;

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
}
bool file::seek_file_pointer(offset_seek_type posDif)
{
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
        switch (file_open_mode(*arguments[0]))
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
}
void file_stream::_outDevice()
{
}

// rtypes::file_binary_stream
bool file_binary_stream::_inDevice() const
{
}
void file_binary_stream::_outDevice()
{
}
