/* rstdio_win32.cpp - implements rstdio using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include Windows API headers
#include <Windows.h>
using namespace rtypes;

// rtypes::standard_device
bool standard_device::open_error(const char*)
{
    HANDLE hStdHandle;
    hStdHandle = ::GetStdHandle(STD_ERROR_HANDLE);
    _error = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    return true;
}
bool standard_device::clear_screen()
{
    // (as a convinience) attempt to clear
    // the screen; I use FillConsoleOutputCharacter
    // and pass it a huge number so that it fills
    // up every character cell
    if (_output != NULL)
    {
        DWORD dummy;
        COORD loc = {0,0};
        if ( ::FillConsoleOutputCharacter(_output->interpret_as<HANDLE>(),' ',0xffffffff,loc,&dummy) )
        {
            // be friendly and reset the cursor to the top
            ::SetConsoleCursorPosition(_output->interpret_as<HANDLE>(),loc);
            return true;
        }
    }
    return false;
}
void standard_device::_writeErrBuffer(const void* buffer,size_type length)
{
    if (_error != NULL)
    {
        DWORD bytesWrote;
        if ( !::WriteFile(_error->interpret_as<HANDLE>(),buffer,length,&bytesWrote,NULL) )
        {
            _lastOp = bad_write;
            _byteCount = 0;
        }
        else if (bytesWrote == 0)
        {
            _lastOp = no_output;
            _byteCount = 0;
        }
        else
        {
            _lastOp = success_write;
            _byteCount = bytesWrote;

        }
    }
    else
    {
        _lastOp = no_device;
        _byteCount = 0;
    }
}
void standard_device::_openEvent(const char*,io_access_flag kind,dword**,dword)
{
    HANDLE hStdHandle;
    if (kind & read_access)
    {
        hStdHandle = ::GetStdHandle(STD_INPUT_HANDLE);
        _input = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    }
    if (kind & write_access)
    {
        hStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        _output = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    }
    if (kind == all_access)
    {
        hStdHandle = ::GetStdHandle(STD_ERROR_HANDLE);
        _error = new io_resource( reinterpret_cast<void*>(hStdHandle),false );
    }
}

// rtypes::standard_stream
bool standard_stream::_inDevice() const
{
    /* An rstream uses \n only to
     * represent end lines; any \r
     * octet is dropped from the input
     * stream. This way, blocks can be
     * read in efficiently in case the 
     * underlying device has a lot of
     * data.
     */
    dword i;
    dword cnt;
    char buffer[4096];
    // read in a buffer
    _device->read(buffer,4096);
    cnt = _device->get_last_byte_count();
    i = 0;
    while (i < cnt)
    {
        const char* pbuf = buffer+i;
        dword len = 0;
        while (i<cnt && pbuf[len]!='\r')
            ++len, ++i;
        _bufIn.push_range(pbuf,len);
        ++i;
    }
    // return success if at least some bytes where read
    return cnt > 0;
}
void standard_stream::_outDevice()
{
    /* On Windows, the \r\n endline
     * encoding is commonly used on many
     * devices. Therefore, all \n characters
     * are translated into the sequence \r\n.
     */
    dword iter = 0;
    const char* pbuffer = &_bufOut.peek();
    while (true)
    {
        dword length = 0;
        while (iter<_bufOut.size() && pbuffer[length] != '\n')
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

// rtypes::standard_binary_stream
bool standard_binary_stream::_inDevice() const
{
    /* don't do anything fancy with the bytes... */
    dword cnt;
    char buffer[4096];
    // read in a buffer
    _device->read(buffer,4096);
    cnt = _device->get_last_byte_count();
    _bufIn.push_range(buffer,cnt);
    // return success if at least some bytes were read
    return cnt > 0;
}
void standard_binary_stream::_outDevice()
{
    /* don't do anything fancy with the bytes... */
    _device->write(&_bufOut.peek(),_bufOut.size());
    _bufOut.clear();
}
