/* rfilename_win32.cpp - implements rfilename using the Windows API
 *  This file should never be targeted directly; it's merely an implementation file referenced conditionally.
 */

// include Windows API headers
#include <Windows.h>
using namespace rtypes;

// Windows supports both kinds of path separators, however
// I'll opt to use this one because it's most commonly
// seen in presentation.
static const char PATH_SEP = '\\';

namespace {
    void set_timestamp_from_stime(timestamp& ts,LPSYSTEMTIME pst)
    {
        ts.year = pst->wYear;
        ts.month = pst->wMonth;
        ts.weekDay = pst->wDayOfWeek;
        ts.day = pst->wDay;
        ts.hour = pst->wHour;
        ts.minute = pst->wMinute;
        ts.second = pst->wSecond;
        ts.millisecond = pst->wMilliseconds;
    }
    void conv_filetime(PFILETIME pft,PSYSTEMTIME pst)
    {
        FILETIME ft;
        ::FileTimeToLocalFileTime(pft,&ft);
        ::FileTimeToSystemTime(&ft,pst);
    }
    inline
    bool case_insens_comp(char a,char b)
    {
        if (a>='A' && a<='Z')
            a -= 'A', a += 'a';
        if (b>='A' && b<='Z')
            b -= 'A', b += 'a';
        return a == b;
    }
    inline
    bool is_drive_spec(const char* pstr) // assume pstr points to at least two characters
    {
        return ((pstr[0]>='A' && pstr[0]<='Z') || (pstr[0]>='a' && pstr[0]<='z')) && pstr[1]==':';
    }
}

// rtypes::file_entry
file_entry_kind file_entry::get_kind() const
{
    if (fileMode != INVALID_FILE_ATTRIBUTES)
    {
        if (fileMode & FILE_ATTRIBUTE_DIRECTORY)
            return kind_directory;
        else
            return kind_regular_file;
    }
    return kind_something_else;
}
void file_entry::_load(const char* pname)
{
    // open the file for reading in order to obtain information about it
    HANDLE hFile = ::CreateFile(pname,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile != INVALID_HANDLE_VALUE) // file opened successfully
    {
        BY_HANDLE_FILE_INFORMATION info;
        name = pname;
        rutil_def_memory(&changeTime,sizeof(timestamp));
        // try to obtain file information
        if ( ::GetFileInformationByHandle(hFile,&info) )
        {
            SYSTEMTIME st;
            conv_filetime(&info.ftCreationTime,&st);
            set_timestamp_from_stime(createTime,&st);
            conv_filetime(&info.ftLastAccessTime,&st);
            set_timestamp_from_stime(lastAccessTime,&st);
            conv_filetime(&info.ftLastWriteTime,&st);
            set_timestamp_from_stime(lastModifyTime,&st);
            fileMode = info.dwFileAttributes;
            fileSize = info.nFileSizeLow;
            fileSize |= qword(info.nFileSizeHigh) << 32;
        }
        else
        {
            rutil_def_memory(&createTime,sizeof(timestamp));
            rutil_def_memory(&lastAccessTime,sizeof(timestamp));
            rutil_def_memory(&lastModifyTime,sizeof(timestamp));
            fileSize = 0;
            fileMode = 0;
        }
        ::CloseHandle(hFile);
    }
    else // failure
    {
        // throw generic errors
        rlib_last_error::switch_throw();
    }
}

// rtypes::path
bool path::exists() const
{
    DWORD a;
    a = ::GetFileAttributes( get_full_name().c_str() );
    return a!=INVALID_FILE_ATTRIBUTES && ((a&FILE_ATTRIBUTE_DIRECTORY) != 0);
}
bool path::is_empty() const
{
    WIN32_FIND_DATA fdata;
    HANDLE hFound;
    str path;
    // prepare path - get everything from the directory
    ((path += get_full_name()) += PATH_SEP) += '*';
    // get directory listing information
    hFound = ::FindFirstFile(path.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        // count items; stop after two to account for
        // the "." and ".." directory items present in
        // every directory
        int cnt = 1;
        while ( cnt<=2 && ::FindNextFile(hFound,&fdata)!=0 )
            ++cnt;
        ::FindClose(hFound);
        if ( ::GetLastError() != ERROR_NO_MORE_FILES )
            throw rlib_err_message("error ::FindNextFile");
        return cnt <= 2;
    }
    // throw generic errors
    rlib_last_error::switch_throw();
    throw 1; // (stop control reaching end of this function)
}
int path::count_elements() const
{
    WIN32_FIND_DATA fdata;
    HANDLE hFound;
    str path;
    // prepare path - get everything from the directory
    ((path += get_full_name()) += PATH_SEP) += '*';
    // get directory listing information
    hFound = ::FindFirstFile(path.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        // count items in directory
        int cnt = 0;
        do {
            ++cnt;
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        cnt -= 2; // disregard "." and ".."
        ::FindClose(hFound);
        return cnt;
    }
    return exists() ? 0 : -1;
}
bool path::make(bool createSubDirectories) const
{
    if (createSubDirectories)
    {
        path p( get_parent_name() );
        p.make(true); // maybe exists, maybe doesn't
    }
    if ( ::CreateDirectory(get_full_name().c_str(),NULL) == 0 )
    {
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
    // check overwrite rule
    str destination = p.get_full_name();
    if ( !overwrite && /*does exist*/::GetFileAttributes( destination.c_str() )!=INVALID_FILE_ATTRIBUTES )
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    // attempt to rename
    if ( !::MoveFile( get_full_name().c_str(),destination.c_str() ) )
    {
        rlib_last_error::switch_set();
        return false;
    }
    if (keepNewName)
        *this = p;
    return true;
}
bool path::rename(const char* pname,bool keepNewName,bool overwrite)
{
    // check to see if the resource exists as a directory
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    // check overwrite rule
    if ( !overwrite && /*does exist*/::GetFileAttributes( pname )!=INVALID_FILE_ATTRIBUTES )
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    // attempt to rename
    if ( !::MoveFile( get_full_name().c_str(),pname ) )
    {
        rlib_last_error::switch_set();
        return false;
    }
    if (keepNewName)
        *this = pname;
    return true;
}
bool path::rename(const generic_string& name,bool keepNewName,bool overwrite)
{
    // check to see if the resource exists as a directory
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    // check overwrite rule
    if ( !overwrite && /*does exist*/::GetFileAttributes( name.c_str() )!=INVALID_FILE_ATTRIBUTES )
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    // attempt to rename
    if ( !::MoveFile( get_full_name().c_str(),name.c_str() ) )
    {
        rlib_last_error::switch_set();
        return false;
    }
    if (keepNewName)
        *this = name;
    return true;
}
bool path::remove()
{
    if ( ::RemoveDirectory( get_full_name().c_str() ) == 0 )
    {
        rlib_last_error::switch_set();
        return false;
    }
    return true;
}
bool path::erase(bool failOnSubDirectories)
{
    HANDLE hFound;
    WIN32_FIND_DATA fdata;
    str pname, searchStr;
    // prepare path - get everything from the directory
    pname = get_full_name();
    (searchStr += PATH_SEP) += '*';
    hFound = ::FindFirstFile(searchStr.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        do {
            if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ( rutil_strcmp(fdata.cFileName,CUR_DIR) || rutil_strcmp(fdata.cFileName,PREV_DIR) )
                    continue; // ignore "." and ".."
                if (!failOnSubDirectories)
                {
                    // recursively remove the sub-directory
                    path p(fdata.cFileName,pname);
                    if ( !p.erase(true) )
                    {
                        ::FindClose(hFound);
                        return false;
                    }
                }
                else
                {
                    rlib_last_error::set("The directory could not be removed because it contained a sub-directory.");
                    ::FindClose(hFound);
                    return false;
                }
            }
            else
            {
                // base case: delete each file (non-directory item)
                if ( !filename(*this,fdata.cFileName).deletef() )
                    return false;
            }
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        ::FindClose(hFound);
        // try to remove the directory
        return remove();
    }
    rlib_last_error::switch_set();
    return false;
}
bool path::get_complete_listing(listing& dirs) const
{
    HANDLE hFound;
    WIN32_FIND_DATA fdata;
    str pname = get_full_name();
    str searchStr = pname;
    (searchStr += PATH_SEP) += '*';
    hFound = ::FindFirstFile(searchStr.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        do {
            if (fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY && (rutil_strcmp(fdata.cFileName,CUR_DIR) || rutil_strcmp(fdata.cFileName,PREV_DIR)))
                continue; // ignore "." and ".." items
            dirs.push_back( ((pname += PATH_SEP) += fdata.cFileName) );
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        ::FindClose(hFound);
    }
    rlib_last_error::switch_set();
    return false;
}
bool path::get_complete_listing(simple_listing& dirs) const
{
    HANDLE hFound;
    WIN32_FIND_DATA fdata;
    str searchStr = get_full_name();
    (searchStr += PATH_SEP) += '*';
    hFound = ::FindFirstFile(searchStr.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        do {
            if (fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY && (rutil_strcmp(fdata.cFileName,CUR_DIR) || rutil_strcmp(fdata.cFileName,PREV_DIR)))
                continue; // ignore "." and ".." items
            dirs.push_back(fdata.cFileName);
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        ::FindClose(hFound);
    }
    rlib_last_error::switch_set();
    return false;
}
bool path::get_subdirectory_listing(listing& dirs) const
{
    HANDLE hFound;
    WIN32_FIND_DATA fdata;
    str pname = get_full_name();
    str searchStr = pname;
    (searchStr += PATH_SEP) += '*';
    hFound = ::FindFirstFile(searchStr.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        do {
            if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ( rutil_strcmp(fdata.cFileName,CUR_DIR) || rutil_strcmp(fdata.cFileName,PREV_DIR) )
                    continue; // ignore "." and ".." items
                dirs.push_back( ((pname += PATH_SEP) += fdata.cFileName) );
            }
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        ::FindClose(hFound);
    }
    rlib_last_error::switch_set();
    return false;
}
bool path::get_subdirectory_listing(simple_listing& dirs) const
{
    HANDLE hFound;
    WIN32_FIND_DATA fdata;
    str searchStr = get_full_name();
    (searchStr += PATH_SEP) += '*';
    hFound = ::FindFirstFile(searchStr.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        do {
            if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ( rutil_strcmp(fdata.cFileName,CUR_DIR) || rutil_strcmp(fdata.cFileName,PREV_DIR) )
                    continue; // ignore "." and ".." items
                dirs.push_back(fdata.cFileName);
            }
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        ::FindClose(hFound);
    }
    rlib_last_error::switch_set();
    return false;
}
bool path::get_file_listing(listing& dirs) const
{
    HANDLE hFound;
    WIN32_FIND_DATA fdata;
    str pname = get_full_name();
    str searchStr = pname;
    (searchStr += PATH_SEP) += '*';
    hFound = ::FindFirstFile(searchStr.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        do {
            if ((fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0)
                dirs.push_back( ((pname += PATH_SEP) += fdata.cFileName) );
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        ::FindClose(hFound);
    }
    rlib_last_error::switch_set();
    return false;
}
bool path::get_file_listing(simple_listing& dirs) const
{
    HANDLE hFound;
    WIN32_FIND_DATA fdata;
    str searchStr = get_full_name();
    (searchStr += PATH_SEP) += '*';
    hFound = ::FindFirstFile(searchStr.c_str(),&fdata);
    if (hFound != INVALID_HANDLE_VALUE)
    {
        do {
            if ((fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0)
                dirs.push_back(fdata.cFileName);
        } while ( ::FindNextFile(hFound,&fdata) != 0 );
        ::FindClose(hFound);
    }
    rlib_last_error::switch_set();
    return false;
}
bool path::set_directory_mode(dword modeBits)
{
    str name = get_full_name();
    DWORD mode = ::GetFileAttributes( name.c_str() );
    bool state = false;
    if (mode!=INVALID_FILE_ATTRIBUTES && (mode&FILE_ATTRIBUTE_DIRECTORY)==0)
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    if (modeBits & file_attribute_hidden)
    {
        mode |= FILE_ATTRIBUTE_HIDDEN;
        state = true;
    }
    else
        mode ^= FILE_ATTRIBUTE_HIDDEN;
    if (modeBits & file_attribute_readonly)
    {
        mode |= FILE_ATTRIBUTE_READONLY;
        state = true;
    }
    else
        mode ^= FILE_ATTRIBUTE_READONLY;
    if (!state && (modeBits&file_attribute_normal))
        mode |= FILE_ATTRIBUTE_NORMAL; // only can apply if is only attribute
    // else // won't run because file_attribute_normal==0x0000
    //     mode ^= FILE_ATTRIBUTE_NORMAL;
    return ::SetFileAttributes( name.c_str(),mode) != 0;
}
/* static */ void path::set_working_path(const path& p)
{
    if ( ::SetCurrentDirectory(p.get_full_name().c_str()) == 0 )
        rlib_last_error::switch_throw(); // throw an error on failure
}
void path::_checkParts()
{
    // remove trailing path separators
    dword sz;
    if (_parts[0].size() > 0)
    {
        bool b = _parts[0].length()>2 && is_drive_spec(_parts[0].c_str());
        sz = _parts[0].size()-1;
        while (sz>dword(b?2:0) && _parts[0][sz]==PATH_SEP)
            --sz;
        _parts[0].truncate(sz+1);
    }
    if (_parts[1].size() > 0)
    {
        bool b = _parts[1].length()>2 && is_drive_spec(_parts[1].c_str());
        sz = _parts[1].size()-1;
        while (sz>dword(b?2:0) && _parts[1][sz]==PATH_SEP)
            --sz;
        _parts[1].truncate(sz+1);
    }
    // check to see if an absolute part was provided
    // check for \ after drive specification
    if ( _parts[0].length()>=2 && is_drive_spec(_parts[0].c_str()) ) // e.g. c:folder OR c:\folder
    {
        // see if the specified path uses the current drive syntax e.g. c:folder
        // if so, place take the drive spec. off of _parts[0]
        if (_parts[0][1]==':' && (_parts[0].length()<=2 || _parts[0][2]!=PATH_SEP)) // not an absolute path
        {
            dword i = 0;
            str temp( _parts[0].size()-2 );
            for (dword j = 2;j<_parts[0].size();i++,j++)
                temp[i] = _parts[0][j];
            _parts[0] = temp;
            _parts[1] = _getWorkDir();
        }
        else if (_parts[1].length() > 0)
            throw undefined_operation_error(); // two absolute paths were provided
    }
    else if (_parts[1].length() == 0)
    {
        _parts[1] = _getWorkDir();
        // see if the specified path is absolute but relative to the current
        // drive; if so, just keep the drive letter and the separating colon
        if (_parts[0].length()>0 && _parts[0][0]==PATH_SEP) // e.g. \folder
            _parts[1].truncate(2); // e.g. c:\current\directory => c:
    }
    else if ( _parts[1].length()<2 || !is_drive_spec(_parts[1].c_str()) )
    {
        str relativeTo = _getWorkDir();
        if (_parts[1][0] == PATH_SEP) // e.g. \folder
        {
            // path is relative to current drive; truncate to just get drive
            // letter and separating colon
            relativeTo.truncate(2); // e.g. c:\current\directory => c:
        }
        else
            relativeTo += PATH_SEP;
        relativeTo += _parts[1];
        _parts[1] = relativeTo;
    }
    else if (_parts[1].length()==2 || /*len>2*/_parts[1][2]!=PATH_SEP)
    {
        // uses current drive syntax e.g. c:folder
        str temp = _getWorkDir();
        temp += PATH_SEP;
        for (dword i = 2;i<_parts[1].length();i++)
            temp.push_back(_parts[1][i]);
        _parts[1] = temp;
    }
}
/* static */ str path::_getWorkDir()
{
    dword len;
    str curDir(MAX_PATH);
    len = ::GetCurrentDirectory(curDir.allocation_size(),&curDir[0]);
    if (len > curDir.allocation_size())
    {
        // resize buffer
        curDir.resize(len-1); // 'len' includes null-terminator
        ::GetCurrentDirectory(curDir.allocation_size(),&curDir[0]);
    }
    else if (len > 0)
        curDir.resize(len); // 'len' does not include null-terminator
    else // failure
        throw rlib_error( int(::GetLastError()) );
    return curDir;
}
/* static */ str path::_getRel(const generic_string& origin,const generic_string& destination)
{
    // get the origin path as it would appear relative to the specified
    // destination path string; assume that both path strings are fully
    // qualified absolute path name strings
    str result;
    const char* pOrigin = origin.c_str();
    const char* pDestination = destination.c_str();
    while ( *pOrigin && *pDestination && case_insens_comp(*pOrigin,*pDestination) ) // NTFS file names are case-insensitive
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
    DWORD a;
    a = ::GetFileAttributes( get_full_name().c_str() );
    return a!=INVALID_FILE_ATTRIBUTES && ((a&FILE_ATTRIBUTE_DIRECTORY) == 0);
}
bool filename::has_contents() const
{
    // have to open file, get its size, then close it
    HANDLE hFile;
    hFile = ::CreateFile(get_full_name().c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER li;
        if ( !::GetFileSizeEx(hFile,&li) )
            rlib_last_error::switch_throw();
        ::CloseHandle(hFile);
        return li.QuadPart > 0;
    }
    // throw generic errors
    rlib_last_error::switch_throw();
    return false; // return on all code paths (won't run)
}
qword filename::size() const
{
    // have to open file, get its size, then close it
    HANDLE hFile;
    hFile = ::CreateFile(get_full_name().c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER li;
        if ( !::GetFileSizeEx(hFile,&li) )
            rlib_last_error::switch_throw();
        ::CloseHandle(hFile);
        return li.QuadPart;
    }
    // throw generic errors
    rlib_last_error::switch_throw();
    return 0; // return on all code paths (won't run)
}
bool filename::create() const
{
    // attempt to create the file only
    // if it does not already exist
    HANDLE hFile;
    hFile = ::CreateFile(get_full_name().c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        rlib_last_error::switch_set();
        return false;
    }
    ::CloseHandle(hFile);
    return true;
}
bool filename::copy(const char* pfname,bool overwrite) const
{
    static const dword BUF_SIZE = 65000;
    byte* buffer;
    HANDLE hSource, hDest;
    bool success;
    // open this file
    hSource = ::CreateFile(get_full_name().c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hSource == INVALID_HANDLE_VALUE)
    {
        rlib_last_error::switch_set();
        return false;
    }
    // open the destination file; overwrite if specified and necessary
    hDest = ::CreateFile(pfname,GENERIC_WRITE,FILE_SHARE_READ,NULL,(overwrite?CREATE_ALWAYS:CREATE_NEW),FILE_ATTRIBUTE_NORMAL,NULL);
    if (hDest == INVALID_HANDLE_VALUE)
    {
        rlib_last_error::switch_set();
        ::CloseHandle(hSource);
        return false;
    }
    // create a buffer on which to perform the copy operation
    buffer = new byte[BUF_SIZE];
    // copy data from source to destination
    success = true;
    while (true)
    {
        DWORD cnt;
        if ( !::ReadFile(hSource,buffer,BUF_SIZE,&cnt,NULL) )
            break;
        if ( !::WriteFile(hDest,buffer,cnt,&cnt,NULL) )
        {
            rlib_last_error::switch_set();
            success = false;
            break;
        }
    }
    delete[] buffer;
    ::CloseHandle(hSource);
    ::CloseHandle(hDest);
    return success;
}
bool filename::rename(const char* pfname,bool keepNewName,bool overwrite)
{
    // check to see if the resource exists as a regular file
    if ( !exists() )
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    // check overwrite condition
    if ( !overwrite && /*does exist*/::GetFileAttributes(pfname)!=INVALID_FILE_ATTRIBUTES )
    {
        rlib_last_error::set<already_exists_error>();
        return false;
    }
    // attempt to rename
    if ( !::MoveFile( get_full_name().c_str(),pfname ) )
    {
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
    return ::DeleteFile( get_full_name().c_str() ) != 0;
}
bool filename::set_file_mode(dword bits)
{
    str name = get_full_name();
    DWORD mode = ::GetFileAttributes( name.c_str() );
    bool state = false;
    if (mode!=INVALID_FILE_ATTRIBUTES && (mode&FILE_ATTRIBUTE_DIRECTORY)!=0)
    {
        rlib_last_error::set<does_not_exist_error>();
        return false;
    }
    if (bits & file_attribute_hidden)
    {
        mode |= FILE_ATTRIBUTE_HIDDEN;
        state = true;
    }
    else
        mode ^= FILE_ATTRIBUTE_HIDDEN;
    if (bits & file_attribute_readonly)
    {
        mode |= FILE_ATTRIBUTE_READONLY;
        state = true;
    }
    else
        mode ^= FILE_ATTRIBUTE_READONLY;
    if (!state && (bits&file_attribute_normal))
        mode |= FILE_ATTRIBUTE_NORMAL; // only can apply if is only attribute
    // else // won't run because file_attribute_normal==0x0000
    //     mode ^= FILE_ATTRIBUTE_NORMAL;
    return ::SetFileAttributes( name.c_str(),mode) != 0;
}
void filename::_trunLeader(str& source)
{
    dword i = source.length()>0 ? source.length()-1 : 0;
    while (i>0 && source[i]!=PATH_SEP)
        --i;
    if (source[i] == PATH_SEP)
    {
        _name.clear();
        for (dword j = i;j<source.length();j++)
            _name.push_back( source[j] );
        if ((i==2 && is_drive_spec( source.c_str() )) || i==0)
            ++i; // source[i] is not a PATH_SEP but the root directory
    }
    else
        _name = source;
    source.truncate(i);
}
