// rfile_win32.cpp - implements rlibrary/rfile using the Windows API
#include <Windows.h>
using namespace rtypes;

// rtypes::file
void file::_openEvent(const char* deviceID,io_access_flag accessKind,void** arguments,dword count)
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
            _input = new io_resource;
            _input->assign(hFile);
        }
        if (accessKind & write_access)
        {
            _output = new io_resource(_input == NULL);
            _output->assign(hFile);
        }
    }
}
void file::_readAll(generic_string& buffer) const
{
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
