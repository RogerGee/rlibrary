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
    io_resource* output = _getOutputContext();
    if (output != NULL)
    {
        DWORD dummy;
        COORD loc = {0,0};
        if ( ::FillConsoleOutputCharacter(output->interpret_as<HANDLE>(),' ',0xffffffff,loc,&dummy) )
        {
            // be friendly and reset the cursor to the top
            ::SetConsoleCursorPosition(output->interpret_as<HANDLE>(),loc);
            return true;
        }
    }
    return false;
}
void standard_device::_openEvent(const char*,io_access_flag kind,io_resource** pinput,io_resource** poutput,void**,uint32)
{
    HANDLE hStdHandle;
    if (kind & read_access)
    {
        *pinput = new io_resource(false);
        (*pinput)->assign( ::GetStdHandle(STD_INPUT_HANDLE) );
    }
    if (kind & write_access)
    {
        *poutput = new io_resource(false);
        (*poutput)->assign( ::GetStdHandle(STD_OUTPUT_HANDLE) );
    }
    if (kind == all_access)
    {
        _error = new io_resource(false);
        _error->assign( ::GetStdHandle(STD_ERROR_HANDLE) );
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
    uint32 i;
    uint32 cnt;
    char buffer[4096];
    // read in a buffer
    _device->read(buffer,4096);
    cnt = _device->get_last_byte_count();
    i = 0;
    while (i < cnt)
    {
        const char* pbuf = buffer+i;
        uint32 len = 0;
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
    uint32 iter = 0;
    const char* pbuffer = &_bufOut.peek();
    void (standard_stream::* pwrite)(const char*,size_type);
    if (_okind == out)
        pwrite = &standard_stream::write;
    else
        pwrite = &standard_stream::write_error;
    while (true)
    {
        uint32 length = 0;
        while (iter<_bufOut.size() && pbuffer[length] != '\n')
            ++length, ++iter;
        (_device->*pwrite)(pbuffer,length);
        if (iter >= _bufOut.size())
            break;
        (_device->pwrite)("\r\n",2);
        pbuffer += length+1;
        ++iter;
    }
    _bufOut.clear();
}

// rtypes::standard_binary_stream
bool standard_binary_stream::_inDevice() const
{
    /* don't do anything fancy with the bytes... */
    uint32 cnt;
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
    if (_okind == out)
        _device->write(&_bufOut.peek(),_bufOut.size());
    else
        _device->write_error(&_bufOut.peek(),_bufOut.size());
    _bufOut.clear();
}
