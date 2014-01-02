/* rfilename_posix.cpp - implements rfilename using POSIX
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally
 * from rfilename.cpp.
 */

// include POSIX and other system headers
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
using namespace rtypes;

static const char PATH_SEP = '/';

// helpers
namespace {
    void set_timestamp_from_time(timestamp& ts,const time_t& timeVal)
    {
        tm* ptm = ::localtime(&timeVal);
        if (ptm != NULL)
        {
            ts.year = 1900+ptm->tm_year; // get year relative to AD 0000
            ts.month = 1+ptm->tm_mon; // get month in range [1=January,12=December]
            ts.day = ptm->tm_mday;
            ts.weekDay = ptm->tm_wday;
            ts.hour = ptm->tm_hour;
            ts.minute = ptm->tm_min;
            ts.second = ptm->tm_sec;
            ts.millisecond = 0;
        }
        else
            rutil_def_memory(&ts,sizeof(timestamp));
    }
}

// rtypes::file_entry
file_entry_kind file_entry::get_kind() const
{
    switch (fileMode&S_IFMT)
    {
    case S_IFLNK:
        return kind_link;
    case S_IFREG:
        return kind_regular_file;
    case S_IFDIR:
        return kind_directory;
    }
    return kind_something_else;
}
void file_entry::_load(const char* pname)
{
    struct stat st;
    if ( ::stat(pname,&st) == 0 ) // success
    {
        name = pname;
        set_timestamp_from_time(changeTime,st.st_ctime);
        set_timestamp_from_time(lastAccessTime,st.st_atime);
        set_timestamp_from_time(lastModifyTime,st.st_mtime);
        fileSize = st.st_size;
        fileMode = st.st_mode;
    }
    else // failure
    {
        // throw common errors
        rlib_last_error::switch_throw();
    }
}

// rtypes::path
bool path::exists() const
{
    struct stat st;
    if ( ::stat(get_full_name().c_str(),&st) != 0 )
    {
        if (errno == ENOENT)
            return false;
        // throw generic errors
        rlib_last_error::switch_throw();
    }
    return (st.st_mode&S_IFMT) == S_IFDIR;
}
bool path::is_empty() const
{
    DIR* direct = ::opendir( get_full_name().c_str() );
    if (direct != NULL)
    {
        // count the items in the directory; there will be at least two, the .
        // and the .. directory items
        int cnt = 0;
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (++cnt > 2)
            {
                ::closedir(direct);
                return false;
            }
        }
        ::closedir(direct);
        return true;
    }
    // throw generic errors
    rlib_last_error::switch_throw();
    throw 1; // (stop control reaching end of this function)
}
int path::count_elements() const
{
    DIR* direct = ::opendir( get_full_name().c_str() );
    if (direct != NULL)
    {
        int cnt = 0;
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            cnt++;
        }
        cnt -= 2; // disregard "." and ".."
        ::closedir(direct);
        return cnt;
    }
    return -1;
}
bool path::make(bool createSubDirectories) const
{
    if (createSubDirectories)
    {
        // recursively create sub-directories
        path parent( get_parent_name() );
        if ( !parent.exists() )
            parent.make(true);
    }
    if ( ::mkdir( get_full_name().c_str(),0777 ) != 0 )
    {
        // set specific errors
        if (errno == ENOENT)
            rlib_last_error::set<bad_path_error>();
        else if (errno == EPERM)
            rlib_last_error::set<directory_creation_not_supported_error>();
        // set generic errors
        else
            rlib_last_error::switch_set();
        return false;
    }
    return true;
}
bool path::copy(const path& p,bool moveContents,bool overwrite) const
{
    // check to make sure this path refers to a directory in the filesystem
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    // check destination existence and overwrite rules
    if ( !p.exists() )
    {
        if ( !p.make() )
            return false;
    }
    else if (!overwrite)
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    if (moveContents)
    {
        str thisName = get_full_name();
        DIR* direct = ::opendir( thisName.c_str() );
        if (direct != NULL)
        {
            while (true)
            {
                dirent* dirEntry = ::readdir(direct);
                if (dirEntry == NULL)
                    break;
                if (dirEntry->d_type==DT_DIR && (rutil_strcmp(dirEntry->d_name,CUR_DIR) || rutil_strcmp(dirEntry->d_name,PREV_DIR)))
                    continue;
                // item is not . or ..
                // attempt the copy but do not care about success
                // (the reasoning being that success and failure could both happen
                // depending on things such as file permissions, ETC.)
                if (dirEntry->d_type == DT_DIR)
                {
                    // recursively copy directory contents
                    path rpath(dirEntry->d_name,thisName.c_str()); // construct path relative-to this path
                    rpath.copy(p+dirEntry->d_name,moveContents,overwrite);
                }
                else if (dirEntry->d_type == DT_REG)
                {
                    filename fn(*this,dirEntry->d_name);
                    fn.copy(p,overwrite);
                }
            }
            ::closedir(direct);
        }
        else
        {
            // set generic errors
            rlib_last_error::switch_set();
            return false;
        }
        
    }
    return true;
}
bool path::rename(const path& p,bool keepNewName,bool overwrite)
{
    // check to see if the resource exists as a directory
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    str destination = p.get_full_name();
    // check overwrite rule
    if (!overwrite && /*does exist*/::access(destination.c_str(),F_OK)==0)
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    if ( ::rename( get_full_name().c_str(),destination.c_str() ) != 0 )
    {
        // set specific errors
        if (errno==EINVAL || errno==EISDIR)
            rlib_last_error::set<bad_path_error>();
        // set generic errors
        else
            rlib_last_error::switch_set();
        return false;
    }
    if (keepNewName)
        *this = p;
    return true;
}
bool path::rename(const char* ps,bool keepNewName,bool overwrite)
{
    // check to see if the resource exists as a directory
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    // check overwrite rule
    if (!overwrite && /*does exist*/::access(ps,F_OK)==0)
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    // attempt to rename
    if ( ::rename( get_full_name().c_str(),ps ) != 0 )
    {
        // set specific errors
        if (errno==EINVAL || errno==EISDIR)
            rlib_last_error::set<bad_path_error>();
        // set generic errors
        else
            rlib_last_error::switch_set();
        return false;
    }
    if (keepNewName)
        *this = ps;
    return true;
}
bool path::rename(const generic_string& s,bool keepNewName,bool overwrite)
{
    // check to see if the resource exists as a directory
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    // check overwrite rule
    if (!overwrite && /*does exist*/::access(s.c_str(),F_OK)==0)
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    // attempt to rename
    if ( ::rename( get_full_name().c_str(),s.c_str() ) != 0 )
    {
        // set specific errors
        if (errno==EINVAL || errno==EISDIR)
            rlib_last_error::set<bad_path_error>();
        // set generic errors
        else
            rlib_last_error::switch_set();
        return false;
    }
    if (keepNewName)
        *this = s;
    return true;
}
bool path::remove()
{
    // only remove the directory if it is not empty
    if ( ::rmdir( get_full_name().c_str() ) != 0 )
    {
        // set specific errors
        if (errno == EINVAL)
            rlib_last_error::set<bad_path_error>();
        // set generic errors
        else
            rlib_last_error::switch_set();
        return false;
    }
    return true;
}
bool path::erase(bool failOnSubDirectories)
{
    str name = get_full_name();
    DIR* direct = ::opendir( name.c_str() );
    if (direct != NULL)
    {
        // erase contents in directory
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (dirEntry->d_type == DT_DIR)
            {
                if ( rutil_strcmp(dirEntry->d_name,CUR_DIR) || rutil_strcmp(dirEntry->d_name,PREV_DIR) )
                    continue;
                if (failOnSubDirectories)
                {
                    rlib_last_error::set("The directory could not be removed because it contained a sub-directory.");
                    ::closedir(direct);
                    return false;
                }
                // recursively delete the sub-directory
                path p = *this;
                p += dirEntry->d_name;
                if ( !p.erase(true) )
                {
                    ::closedir(direct);
                    return false;
                }
            }
            else
            {
                // base case: delete each file (non-directory item)
                if ( !filename(*this,dirEntry->d_name).deletef() )
                    return false;
            }
        }
        ::closedir(direct);
        // try to remove the directory
        return remove();
    }
    // set generic errors
    rlib_last_error::switch_set();
    return false;
}
bool path::get_complete_listing(listing& dirs) const
{
    str pname = get_full_name();
    DIR* direct = ::opendir( pname.c_str() );
    if (direct != NULL)
    {
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (dirEntry->d_type==DT_DIR && (rutil_strcmp(dirEntry->d_name,CUR_DIR) || rutil_strcmp(dirEntry->d_name,PREV_DIR)))
                continue;
            dirs.push_back( ((pname += PATH_SEP) += dirEntry->d_type) );
        }
        ::closedir(direct);
        return true;
    }
    // set generic errors
    rlib_last_error::switch_set();
    return false;
}
bool path::get_complete_listing(simple_listing& dirs) const
{
    DIR* direct = ::opendir( get_full_name().c_str() );
    if (direct != NULL)
    {
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (dirEntry->d_type==DT_DIR && (rutil_strcmp(dirEntry->d_name,CUR_DIR) || rutil_strcmp(dirEntry->d_name,PREV_DIR)))
                continue;
            dirs.push_back(dirEntry->d_name);
        }
        ::closedir(direct);
        return true;
    }
    // set generic errors
    rlib_last_error::switch_set();
    return false;
}
bool path::get_subdirectory_listing(listing& dirs) const
{
    str pname = get_full_name();
    DIR* direct = ::opendir( get_full_name().c_str() );
    if (direct != NULL)
    {
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (dirEntry->d_type==DT_DIR && !rutil_strcmp(dirEntry->d_name,CUR_DIR) && !rutil_strcmp(dirEntry->d_name,PREV_DIR))
                dirs.push_back( ((pname += PATH_SEP) += dirEntry->d_type) );
        }
        ::closedir(direct);
        return true;
    }
    // set generic errors
    rlib_last_error::switch_set();
    return false;
}
bool path::get_subdirectory_listing(simple_listing& dirs) const
{
    DIR* direct = ::opendir( get_full_name().c_str() );
    if (direct != NULL)
    {
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (dirEntry->d_type==DT_DIR && !rutil_strcmp(dirEntry->d_name,CUR_DIR) && !rutil_strcmp(dirEntry->d_name,PREV_DIR))
                dirs.push_back(dirEntry->d_name);
        }
        ::closedir(direct);
        return true;
    }
    // set generic errors
    rlib_last_error::switch_set();
    return false;
}
bool path::get_file_listing(listing& dirs) const
{
    str pname = get_full_name();
    DIR* direct = ::opendir( get_full_name().c_str() );
    if (direct != NULL)
    {
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (dirEntry->d_type == DT_REG)
                dirs.push_back( ((pname += PATH_SEP) += dirEntry->d_type) );
        }
        ::closedir(direct);
        return true;
    }
    // set generic errors
    rlib_last_error::switch_set();
    return false;
}
bool path::get_file_listing(simple_listing& dirs) const
{
    DIR* direct = ::opendir( get_full_name().c_str() );
    if (direct != NULL)
    {
        while (true)
        {
            dirent* dirEntry = ::readdir(direct);
            if (dirEntry == NULL)
                break;
            if (dirEntry->d_type == DT_REG)
                dirs.push_back(dirEntry->d_name);
        }
        ::closedir(direct);
        return true;
    }
    // set generic errors
    rlib_last_error::switch_set();
    return false;
}
bool path::set_directory_mode(dword modeBits)
{
    if (modeBits & file_attribute_hidden)
    {
        // check to see if filename has a dot before it;
        // directories that explicitly specify . or .. will
        // not be resolved
        str top = get_top_name();
        if (top.length()>0 && top[0]!='.')
        {
            top = get_full_name();
            set_top_name("."+top);
            if ( ::rename(top.c_str(),get_full_name().c_str()) != 0 )
            {
                rlib_last_error::switch_set();
                return false;
            }
        }
    }
    else
    {
        // remove leading dot in hidden file
        str top = get_top_name();
        if (top.length()>0 && top!="." && top!=".." && top[0]=='.')
        {
            dword i = 0;
            while (top[i] == '.')
                i++;
            top = get_full_name();
            set_top_name(top.c_str()+i);
            if ( ::rename(top.c_str(),get_full_name().c_str()) != 0 )
            {
                rlib_last_error::switch_set();
                return false;
            }
        }
    }
    if (modeBits & file_attribute_readonly)
    {
        // remove all write permission bits
        struct stat st;
        str name = get_full_name();
        if ( ::stat(name.c_str(),&st) == 0 )
        {
            if ((st.st_mode&S_IFMT) != S_IFDIR)
            {
                rlib_last_error::set<does_not_exist_error>();
                return false;
            }
            st.st_mode ^= 0444;
            if ( ::chmod(name.c_str(),st.st_mode) != 0 )
            {
                rlib_last_error::switch_set();
                return false;
            }
        }
        else
        {
            rlib_last_error::switch_set();
            return false;
        }
    }
    else
    {
        // set any read bits for which there are write bits
        struct stat st;
        str name = get_full_name();
        if ( ::stat(name.c_str(),&st) == 0 )
        {
            if ((st.st_mode&S_IFMT) != S_IFDIR)
            {
                rlib_last_error::set<does_not_exist_error>();
                return false;
            }
            if (st.st_mode & S_IWUSR)
                st.st_mode |= S_IRUSR;
            if (st.st_mode & S_IWGRP)
                st.st_mode |= S_IRGRP;
            if (st.st_mode & S_IWOTH)
                st.st_mode |= S_IROTH;
            if ( ::chmod(name.c_str(),st.st_mode) != 0 )
            {
                rlib_last_error::switch_set();
                return false;
            }
        }
        else
        {
            rlib_last_error::switch_set();
            return false;
        }
    }
    return true;
}
/* static */ void path::set_working_path(const path& p)
{
    str n = p.get_full_name();
    if ( ::chdir(n.c_str()) != 0 )
    {
        // throw specific errors
        if (errno == EPERM)
            throw access_denied_error();
        // throw generic errors
        rlib_last_error::switch_throw();
    }
}
void path::_checkParts()
{
    // remove trailing path separators
    dword sz;
    if (_parts[0].size() > 0)
    {
        sz = _parts[0].size()-1;
        while (sz>0 && _parts[0][sz]==PATH_SEP)
            --sz;
        _parts[0].truncate(sz+1);
    }
    if (_parts[1].size() > 0)
    {
        sz = _parts[1].size()-1;
        while (sz>0 && _parts[1][sz]==PATH_SEP)
            --sz;
        _parts[1].truncate(sz+1);
    }
    // check to see if an absolute part was provided
    if (_parts[0].length()>0 && _parts[0][0]==PATH_SEP)
    {
        if (_parts[1].length() > 0)
            throw undefined_operation_error(); // two absolute parts were provided; no such operation exists
    }
    else if (_parts[1].length() == 0)
        _parts[1] = _getWorkDir(); // use the current directory by default
    else if (_parts[1][0] != PATH_SEP)
    {
        // make sure the absolute part is absolute
        str relativeTo = _getWorkDir();
        (relativeTo += PATH_SEP) += _parts[1];
        _parts[1] = relativeTo;
    }
}
/* static */ str path::_getWorkDir()
{
    // obtain the current working directory
    str outDir;
    dword i;
    outDir.resize(256);
    while ( ::getcwd(&outDir[0],outDir.allocation_size()) == NULL )
    {
        if (errno != ERANGE)
            throw rlib_error(errno);
        outDir.resize( outDir.capacity()*2 );
    }
    i = 0;
    while (i<outDir.size() && outDir[i]!=0)
        i++;
    outDir.truncate(i);
    return outDir;
}
/* static */str path::_getRel(const generic_string& origin,const generic_string& destination)
{
    // get the origin path as it would appear relative to the specified
    // destination path string; assume that both path strings are fully
    // qualified absolute path name strings
    str result;
    const char* pOrigin = origin.c_str();
    const char* pDestination = destination.c_str();
    while (*pOrigin && *pDestination && *pOrigin==*pDestination) // POSIX has case-sensitive path/file names
        ++pOrigin, ++pDestination;
    if (*pDestination==0 && pDestination!=destination.c_str() && *(pDestination-1)==PATH_SEP)
        --pDestination, --pOrigin; // special case for if path separator at end
    if ((*pDestination==PATH_SEP || *pDestination==0) && (*pOrigin==PATH_SEP || *pOrigin==0))
    {
        // the path led up to the current directory
        if (*pOrigin)
            ++pOrigin;
        while (*pOrigin)
            result.push_back(*pOrigin++);
        if (result.length() == 0)
            result = "."; // the directories are the same
    }
    else
    {
        result = "..";
        if (*pDestination == PATH_SEP)
            ++pDestination;
        while (*pDestination)
        {
            if (*pDestination == PATH_SEP)
                (result += PATH_SEP) += "..";
            ++pDestination;
        }
        while (*pOrigin && *pOrigin!=PATH_SEP)
            --pOrigin; // move back to start of first directory not in destination
        while (*pOrigin)
            result.push_back(*pOrigin++);
    }
    return result;
}

// rtypes::filename
bool filename::exists() const
{
    // see if the name exists as a regular file
    struct stat st;
    if ( ::stat(get_full_name().c_str(),&st) != 0 )
    {
        if (errno == ENOENT)
            return false;
        // throw generic errors
        rlib_last_error::switch_throw();
    }
    return (st.st_mode&S_IFMT) == S_IFREG;
}
bool filename::has_contents() const
{
    // see if the file size is greater than zero
    // the file is assumed to exist, else thrown error
    struct stat st;
    if ( ::stat(get_full_name().c_str(),&st) != 0 )
    {
        // throw generic errors
        rlib_last_error::switch_throw();
    }
    if ((st.st_mode&S_IFMT) == S_IFDIR)
        return st.st_size > 0;
    // was not regular file
    throw does_not_exist_error();
}
qword filename::size() const
{
    struct stat st;
    if ( ::stat(get_full_name().c_str(),&st) != 0 )
    {
        // throw generic errors
        rlib_last_error::switch_throw();
    }
    if ((st.st_mode&S_IFMT) == S_IFDIR)
        return qword(st.st_size);
    // was not regular file
    throw does_not_exist_error();
}
bool filename::create() const
{
    // attempt to create the file, only
    // if it does not already exist
    int fd;
    fd = ::open( get_full_name().c_str(),O_CREAT|O_EXCL|O_RDONLY,0664 );
    if (fd == -1)
    {
        if (errno == EROFS)
            rlib_last_error::set<access_denied_error>();
        else
            rlib_last_error::switch_set();
        return false;
    }
    ::close(fd);
    return true;
}
bool filename::copy(const char* pfname,bool overwrite) const
{
    static const dword BUF_SIZE = 65000;
    byte* buffer;
    int fsource, fnew;
    bool success;
    // open this file
    fsource = ::open(get_full_name().c_str(),O_RDONLY);
    if (fsource == -1)
    {
        rlib_last_error::switch_set();
        return false;
    }
    // open the new file; overwrite if specified and necessary
    fnew = ::open(pfname,O_CREAT|(overwrite?O_EXCL:O_TRUNC)|O_WRONLY,0664);
    if (fnew == -1)
    {
        rlib_last_error::switch_set();
        ::close(fsource);
        return false;
    }
    // create a buffer on which to perform the copy operation
    buffer = new byte[BUF_SIZE];
    // copy data from source to destination
    success = true;
    while (true)
    {
        ssize_t cnt;
        cnt = ::read(fsource,buffer,BUF_SIZE);
        if (cnt <= 0)
            break;
        if ( ::write(fnew,buffer,cnt) == -1 )
        {
            rlib_last_error::switch_set();
            success = false;
            break;
        }
    }
    delete[] buffer;
    ::close(fsource);
    ::close(fnew);
    return success;
}
bool filename::rename(const char* pfname,bool keepNewName,bool overwrite)
{
    // check to see if the file exists as a regular file
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    // check overwrite condition
    if (!overwrite && /*does exist*/::access(pfname,F_OK)==0)
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    if ( ::rename( get_full_name().c_str(),pfname ) != 0 )
    {
        // set specific errors
        if (errno==EINVAL || errno==EISDIR)
            rlib_last_error::set<bad_path_error>();
        // set generic errors
        else
            rlib_last_error::switch_set();
        return false;
    }
    if (keepNewName)
    {
        str s = pfname;
        _trunLeader(s);
        _path = s;
    }
    return true;
}
bool filename::deletef()
{
    if ( ::unlink( get_full_name().c_str() ) != 0 )
    {
        rlib_last_error::switch_set();
        return false;
    }
    return true;
}
bool filename::set_file_mode(dword bits)
{
    if (bits & file_attribute_hidden)
    {
        // add a dot before the file to follow 
        // conventions for hidden files; only add
        // the dot if it does not already exist
        if (_name.length()>0 && _name[0]!='.')
            set_name("."+_name);
    }
    else
    {
        // remove the dot in the file name to
        // "unhide" the hidden file
        if (_name.length()>0 && _name[0]=='.')
        {
            dword i = 0;
            while (i<_name.length() && _name[i]=='.')
                ++i;
            set_name(_name.c_str()+i);
        }
    }
    if (bits & file_attribute_readonly)
    {
        // remove all write permission bits
        struct stat st;
        str name = get_full_name();
        if ( ::stat(name.c_str(),&st) == 0 )
        {
            if ((st.st_mode&S_IFMT) != S_IFDIR)
            {
                rlib_last_error::set<does_not_exist_error>();
                return false;
            }
            st.st_mode ^= 0444;
            if ( ::chmod(name.c_str(),st.st_mode) != 0 )
            {
                rlib_last_error::switch_set();
                return false;
            }
        }
        else
        {
            rlib_last_error::switch_set();
            return false;
        }
    }
    else
    {
         // set any read bits for which there are write bits
        struct stat st;
        str name = get_full_name();
        if ( ::stat(name.c_str(),&st) == 0 )
        {
            if ((st.st_mode&S_IFMT) != S_IFDIR)
            {
                rlib_last_error::set<does_not_exist_error>();
                return false;
            }
            if (st.st_mode & S_IWUSR)
                st.st_mode |= S_IRUSR;
            if (st.st_mode & S_IWGRP)
                st.st_mode |= S_IRGRP;
            if (st.st_mode & S_IWOTH)
                st.st_mode |= S_IROTH;
            if ( ::chmod(name.c_str(),st.st_mode) != 0 )
            {
                rlib_last_error::switch_set();
                return false;
            }
        }
        else
        {
            rlib_last_error::switch_set();
            return false;
        }
    }
    return true;
}
void filename::_trunLeader(str& source)
{
    dword i = source.length()>0 ? source.length()-1 : 0;
    while (i>0 && source[i]!=PATH_SEP)
        --i;
    if (source[i] == PATH_SEP)
    {
        _name.clear();
        for (dword j = i+1;j<source.length();j++)
            _name.push_back( source[j] );
        if (i == 0) // this PATH_SEP is not a separator but the root directory
            ++i;
    }
    else
        _name = source;
    source.truncate(i);
}
