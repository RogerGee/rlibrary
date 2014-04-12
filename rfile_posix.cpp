// rfile_posix - implements rlibrary/rfile targeting POSIX
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
using namespace rtypes;

// rtypes::file
offset_type file::get_file_pointer() const
{
    const io_resource* iores = _getValidContext();
    if (iores != NULL)
    {
        off_t cur;
        cur = ::lseek(iores->interpret_as<int>(),0,SEEK_CUR);
        return cur;
    }
    throw object_not_initialized_error();
}
offset_type file::get_file_size() const
{
    const io_resource* iores = _getValidContext();
    if (iores != NULL)
    {
        off_t size;
        size = ::lseek(iores->interpret_as<int>(),0,SEEK_END);
        return size;
    }
    throw object_not_initialized_error();
}
bool file::eof() const
{
    const io_resource* iores = _getValidContext();
    if (iores != NULL)
    {
        // see if the file pointer currently is
        // at the end of file
        int fd = iores->interpret_as<int>();
        off_t cur, size;
        if ((cur = ::lseek(fd,0,SEEK_CUR))==-1 || (size = ::lseek(fd,0,SEEK_END))==-1)
        {
            if (errno==EBADF || errno==ESPIPE)
                throw invalid_resource_error();
            else if (errno == EINVAL)
                throw undefined_operation_error();
            else
                throw rlib_system_error(errno);
        }
        return cur == size;
    }
    throw object_not_initialized_error();
}
bool file::set_file_pointer(offset_type pos)
{
    io_resource* iores = _getValidContext();
    if (iores != NULL)
        return ::lseek(iores->interpret_as<int>(),pos,SEEK_SET) != -1;
    throw object_not_initialized_error();
}
bool file::seek_file_pointer(offset_seek_type posDif)
{
    io_resource* iores = _getValidContext();
    if (iores != NULL)
        return ::lseek(iores->interpret_as<int>(),posDif,SEEK_CUR);
    throw object_not_initialized_error();
}
bool file::truncate()
{
    io_resource* iores = _getValidContext();
    if (iores != NULL)
        return ::ftruncate(iores->interpret_as<int>(),0) == 0;
    throw object_not_initialized_error();
}
bool file::resize(offset_type size)
{
    io_resource* iores = _getValidContext();
    if (iores != NULL)
        return ::ftruncate(iores->interpret_as<int>(),size) == 0;
    throw object_not_initialized_error();
}
void file::_openEvent(const char* deviceID,io_access_flag accessKind,io_resource** pinput,io_resource** poutput,void** arguments,uint32 count)
{
    bool doesAppend = false;
    int flags = 0;
    mode_t mode = S_IRUSR|S_IWUSR; // use 'user' permissions; (permissions haven't been implemented yet in rlibrary)
    // select access mode based on access flag
    if (accessKind == all_access)
        flags |= O_RDWR;
    else if (accessKind == read_access)
        flags |= O_RDONLY;
    else if (accessKind == write_access)
        flags |= O_WRONLY;
    // select mode based on create disposition
    if (count>=1 && arguments!=NULL)
    {
        switch (*reinterpret_cast<file_open_mode*>(arguments[0]))
        {
        case file_create_exclusively:
            flags |= O_CREAT | O_EXCL;
            break;
        case file_create_always:
            flags |= O_CREAT | O_TRUNC;
            break;
        case file_open_always:
            flags |= O_CREAT;
            break;
        case file_open_append:
            flags |= O_CREAT;
            doesAppend = true;
            break;
        case file_open_existing:
            // default 
            break;
        case file_truncate_existing:
            flags |= O_TRUNC;
            break;
        case file_append_existing:
            doesAppend = true;
            break;
        }
    }
    // attempt to open file
    int fd = ::open(deviceID,flags,mode);
    if (fd == -1)
    {
        // set any generic errors
        rlib_last_error::switch_set();
    }
    else
    {
        if (doesAppend) // attempt to set file pointer to end of file
            ::lseek(fd,0,SEEK_END);
        if (accessKind & read_access)
        {
            *pinput = new io_resource;
            (*pinput)->assign(fd);
        }
        if (accessKind & write_access)
        {
            *poutput = new io_resource(*pinput == NULL);
            (*poutput)->assign(fd);
        }
    }
}
void file::_readAll(generic_string& buffer) const
{
    buffer.resize( get_file_size() );
    read(buffer);
}

// rtypes::file_stream
bool file_stream::_inDevice() const
{
    char buffer[4096];
    _device->set_file_pointer(_ideviceIter);
    _device->read(buffer,4096);
    if (_device->get_last_operation_status() == success_read)
    {
        _bufIn.push_range(buffer,_device->get_last_byte_count());
        _ideviceIter += _device->get_last_byte_count();
        return true;
    }
    return false;
}
void file_stream::_outDevice()
{
    _device->set_file_pointer(_odeviceIter);
    _device->write(&_bufOut.peek(),_bufOut.size());
    _odeviceIter += _bufOut.size();
    _bufOut.clear();
}

// rtypes::file_binary_stream
bool file_binary_stream::_inDevice() const
{
    char buffer[4096];
    _device->set_file_pointer(_ideviceIter);
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
    _device->set_file_pointer(_odeviceIter);
    _device->write(&_bufOut.peek(),_bufOut.size());
    _odeviceIter += _bufOut.size();
    _bufOut.clear();
}
