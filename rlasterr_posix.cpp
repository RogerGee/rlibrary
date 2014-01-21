/* rlasterr_posix.cpp - implements rfilename using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

#include <errno.h>
using namespace rtypes;

/* static */ void rlib_last_error::switch_set()
{
    // set documented errors
    if (errno == EACCES)
        set<access_denied_error>();
    else if (errno==ENOMEM || errno==ENFILE)
        set<out_of_memory_error>();
    else if (errno == ENOENT)
        set<does_not_exist_error>();
    else if (errno==ENOTDIR || errno==ENAMETOOLONG)
        set<bad_path_error>();
    else if (errno == EFAULT)
        set<fault_error>();
    else if (errno == EEXIST)
        set<already_exists_error>();
    else if (errno == ENOSPC)
        set<out_of_space_error>();
    else if (errno == ENOTEMPTY)
        set<directory_not_empty_error>();
    else if (errno == EBUSY)
        set<resource_in_use_error>();
    else if (errno == EIO)
        set<io_error>();
    else if (errno == EBADF)
        set<invalid_resource_error>();
    // set undocumented error
    else
        _sysSet(errno);
}
/* static */ void rlib_last_error::switch_throw()
{
    // throw documented errors
    switch (errno)
    {
    case EACCES:
        throw access_denied_error();
    case ENOMEM:
    case ENFILE:
        throw out_of_memory_error();
    case ENOENT:
        throw does_not_exist_error();
    case ENOTDIR:
    case ENAMETOOLONG:
        throw bad_path_error();
    case EFAULT:
        throw fault_error();
    case EEXIST:
        throw already_exists_error();
    case ENOSPC:
        throw out_of_space_error();
    case ENOTEMPTY:
        throw directory_not_empty_error();
    case EBUSY:
        throw resource_in_use_error();
    case EIO:
        throw io_error();
    case EBADF:
        throw invalid_resource_error();
    }    
    // throw undocumented error
    throw rlib_system_error(errno);
}
