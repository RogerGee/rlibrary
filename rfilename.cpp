/* rfilename.cpp
 *  Compile target platform flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */
#include "rfilename.h" // gets rfilemode.h
#include "rutility.h"
#include "rlasterr.h" // gets rerror.h

// define constants
const char* const CUR_DIR = ".";
const char* const PREV_DIR = "..";

// define target-specific code
//  -> Gets: 'PATH_SEP'
//  -> Imports: 'using namespace rtypes'

#if defined(RLIBRARY_BUILD_POSIX)
#include "rfilename_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "rfilename_win32.cpp"
#else
using namespace rtypes; // import names
#endif

// define target-independent code

// rtypes::file_entry
file_entry::file_entry()
{
    rutil_def_memory(&createTime,sizeof(timestamp));
    rutil_def_memory(&lastAccessTime,sizeof(timestamp));
    rutil_def_memory(&lastModifyTime,sizeof(timestamp));
    fileSize = 0;
    fileMode = 0;
}

// rtypes::filename
path::path()
{
}
path::path(const str& pathName)
{
    _parts[0] = pathName;
    _checkParts();
}
path::path(const str& pathName,const str& relativeTo)
{
    _parts[0] = pathName;
    _parts[1] = relativeTo;
    _checkParts();
}
path& path::operator =(const str& pathName)
{
    _parts[0] = pathName;
    _parts[1].clear();
    _checkParts();
    return *this;
}
path& path::operator +=(const str& component)
{
    if (component.size() > 0)
    {
        if (component[0] != PATH_SEP)
        {
            if (_parts[0].length()>0 && _parts[0][0]!=PATH_SEP)
                _parts[0] += PATH_SEP;
            _parts[0] += component;
            _checkParts();
        }
        else
            throw undefined_operation_error();
    }
    return *this;
}
path& path::operator +=(const path& p)
{
    return this->operator +=(p._parts[0]);
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
bool path::copy(const str& name,bool moveContents,bool overwrite) const
{
    return copy(path(name),moveContents,overwrite);
}
str path::get_name() const
{
    if (_parts[0].length() == 0)
        return _parts[1];
    return _parts[0];
}
str path::get_top_name() const
{
    str r;
    const str& s = _parts[0].length()==0 ? _parts[1] : _parts[0];
    dword i = s.length()>0 ? s.length()-1 : 0;
    while (i>0 && s[i]!=PATH_SEP) // advance iterator to next separator (right-to-left)
        --i;
    if (s[i] == PATH_SEP) // move iterator past sepatator
        ++i;
    for (;i<s.length();i++) // get top level directory
        r += s[i];
    return r;
}
str path::get_full_name() const
{
    str abspath;
    if (_parts[1].length() > 0)
        abspath += _parts[1];
    if (abspath.length()>0 && abspath[abspath.length()-1]!=PATH_SEP && _parts[0].length()>0 && _parts[0][0]!=PATH_SEP)
        abspath += PATH_SEP;
    abspath += _parts[0];
    return abspath;
}
str path::get_relative_name() const
{
    // must pass in fully-qualified names
    return _getRel(get_full_name(),_getWorkDir());
}
str path::get_relative_name(const path& p) const
{
    // must pass in fully-qualified names
    return _getRel(get_full_name(),p.get_full_name());
}
str path::get_parent_name() const
{
    str abspath = get_full_name();
    dword sz = abspath.size();
    while (sz > 0)
        if (abspath[--sz] == PATH_SEP)
            break;
    abspath.truncate(sz==0 ? 1 : sz);
    return abspath;
}
void path::set_name(const str& pathName)
{
    _parts[0] = pathName;
    _checkParts();
}
void path::set_name(const str& pathName,const str& relativeTo)
{
    _parts[0] = pathName;
    _parts[1] = relativeTo;
    _checkParts();
}
void path::set_full_name(const str& n)
{
    // take the top component
    // and put it as the relative part
    dword i = n.size()>0 ? n.size()-1 : 0;
    while (i>0 && n[i]!=PATH_SEP)
        --i;
    _parts[0].clear();
    _parts[1].clear();
    for (dword j = i;j<n.size();j++)
        _parts[0].push_back(n[j]);
    for (dword j = 0;j<i;j++)
        _parts[1].push_back(n[j]);
    _checkParts();
}
void path::set_top_name(const str& n)
{
    str& s = _parts[0].length()==0 ? _parts[1] : _parts[0];
    dword sz = s.size()-1;
    while (sz>0 && s[sz]!=PATH_SEP)
        --sz;
    if (s[sz] == PATH_SEP)
        ++sz;
    s.truncate(sz);
    s += n;
    _checkParts();
}
void path::set_parent_path(const path& p)
{
    _parts[0] = get_top_name();
    _parts[1] = p.get_full_name();
    //_checkParts(); // don't have to do this because 'path' guarentees checks
}
void path::set_relative_path()
{
    _parts[0] = _parts[0].length()>0 && _parts[0][0]!=PATH_SEP ? _parts[0] : get_top_name();
    _parts[1] = _getWorkDir();
    //_checkParts(); // don't have to do this
}
void path::set_relative_path(const path& p)
{
    _parts[0] = _parts[0].length()>0 && _parts[0][0]!=PATH_SEP ? _parts[0] : get_top_name();
    _parts[1] = p.get_full_name();
    //_checkParts(); // don't have to do this because 'path' guarentees checks
}
file_entry path::get_entry_info() const
{
    file_entry entry( get_full_name().c_str() );
    if (entry.get_kind() == kind_directory)
        return entry;
    throw does_not_exist_error();
}
void path::append_name(const str& n)
{
    this->operator +=(n);
}
void path::append_before(const str& n)
{
    str top = get_top_name();
    str par = get_parent_name();
    _parts[0] = top;
    _parts[1].clear();
    if (n.length()>0 && n[0]==PATH_SEP)
        (_parts[1] += par) += n;
    else
        ((_parts[1] += par) += PATH_SEP) += n;
    _checkParts();
}
/* static */ path path::get_working_path()
{
    path p;
    p._parts[0] = _getWorkDir();
    return p;
}

// rtypes::filename
filename::filename()
{
}
filename::filename(const str& name)
{
    str source(name);
    _trunLeader(source);
    _path = source;
}
filename::filename(const str& pathLocation,const str& name)
    : _path(pathLocation)
{
    str source = name;
    _trunLeader(source);
    // should be a relative path string, else an error will be thrown
    _path.append_name(source);
}
filename::filename(const path& pathLocation,const str& name)
    : _path(pathLocation)
{
    str source(name);
    _trunLeader(source);
    // should be a relative path string, else an error will be thrown
    _path.append_name(source);
}
filename& filename::operator =(const str& s)
{
    str source(s);
    _trunLeader(source);
    _path = source;
    return *this;
}
bool filename::has_extension() const
{
    return get_extension(false).length() > 0;
}
bool filename::copy(const path& toDirectory,bool overwrite) const
{
    filename fi(toDirectory,_name);
    return copy(fi.get_full_name(),overwrite);
}
bool filename::copy(const filename& toFile,bool overwrite) const
{
    return copy(toFile.get_full_name(),overwrite);
}
bool filename::rename(const path& p,bool keepNewName,bool overwrite)
{
    filename fi(p,_name);
    return rename(fi.get_full_name(),keepNewName,overwrite);
}
bool filename::rename(const filename& name,bool keepNewName,bool overwrite)
{
    return rename(name.get_full_name(),keepNewName,overwrite);
}
str filename::get_name() const
{
    return _name;
}
str filename::get_namex() const
{
    // get just the name excluding the extension
    str n;
    if (_name.length() > 0)
    {
        dword i = _name.length()-1;
        while (i>0 && _name[i]!='.')
            --i;
        if (_name[i] == '.')
        {
            for (dword j = 0;j<i;j++)
                n.push_back( _name[j] );
        }
        else
            return _name;
    }
    return n;
}
str filename::get_extension(bool includeLeadingDot) const
{
    str ext;
    if (_name.length() > 0)
    {
        dword i = _name.length()-1;
        while (i>0 && _name[i]!='.')
            --i;
        if (i>0 || _name[0]=='.')
        {
            if (!includeLeadingDot)
                ++i;
            for (;i<_name.length();i++)
                ext.push_back( _name[i] );
        }
    }
    return ext;
}
str filename::get_full_name() const
{
    str fn( _path.get_full_name() );
    if (fn.length()>0 && fn[fn.length()-1]!=PATH_SEP)
        fn += PATH_SEP;
    fn += _name;
    return fn;
}
path& filename::get_path()
{
    return _path;
}
const path& filename::get_path() const
{
    return _path;
}
str filename::get_relative_name() const
{
    str fn( _path.get_relative_name() );
    (fn += PATH_SEP) += _name;
    return fn;
}
str filename::get_relative_name(const path& p) const
{
    str fn( _path.get_relative_name(p) );
    (fn += PATH_SEP) += _name;
    return fn;
}
void filename::set_name(const str& name)
{
    _name = name;
}
void filename::set_name(const str& name,const str& extension)
{
    // treat extension as a suffix
    dword start = 0;
    _name = name;
    // move past any dots before the extension
    while (start<extension.length() && extension[start]=='.')
        ++start;
    _name += '.';
    for (;start<extension.length();start++)
        _name += extension[start];
}
void filename::set_namex(const str& name)
{
    // set just the name part of 'name'
    // the extension is the last .suffix
    if (name.length() > 0)
    {
        dword i = name.length()-1;
        while (i>0 && name[i]!='.')
            --i;
        if (name[i] == '.')
        {
            // set name excluding extension
            _name.clear();
            for (dword j = 0;j<i;j++)
                _name.push_back( name[j] );
        }
        else // no extension was specified
            _name = name;
    }
    else // no name specified
        _name.clear();
}
void filename::set_extension(const str& name)
{
    // the extension in 'name' is the last
    // .suffix
    dword insertPos;
    // find the beginning of the current extension
    insertPos = _name.length()>0 ? _name.length()-1 : 0;
    while (insertPos>0 && _name[insertPos]!='.')
        --insertPos;
    if (insertPos<_name.length() && _name[insertPos]!='.')
        insertPos = _name.length();
    // remove the current extension
    _name.truncate(insertPos);
    // look at the specified extension
    if (name.length() > 0)
    {
        // find the beginning of the specified extension
        dword j = name.length()-1;
        while (j>0 && name[j]!='.')
            --j;
        if (name[j] == '.') // found extension
        {
            _name += '.';
            for (;j<name.length();j++) // apply specified extension
                _name.push_back( name[j] );
        }
    }
}
void filename::set_full_name(const str& name)
{
    // the last component of 'name' is assumed
    // to the filename; everything before is assumed
    // to be the path; getting the component is system
    // specific due to root directory issues
    str cpy = name;
    _trunLeader(cpy);
    _path = cpy;
}
void filename::set_path(const path& p)
{
    _path = p;
}
file_entry filename::get_entry_info() const
{
    file_entry entry( get_full_name().c_str() );
    if (entry.get_kind() == kind_regular_file)
        return entry;
    throw does_not_exist_error();
}

path rtypes::operator +(const path& p,const str& s)
{
    path cpy(p);
    cpy += s;
    return cpy;
}
filename rtypes::operator +(const path& p,const filename& fn)
{
    filename r;
    r.set_name( fn.get_name() );
    r.set_path(p);
    return r;
}
