/* rfilename.h
 *  rlibrary/rfilename - contains types that provide functionality
 * for manipulating file and directory names; provides a cross-platform
 * interface for dealing with files and directories
 */
#ifndef RFILENAME_H
#define RFILENAME_H
#include "rstring.h" // gets rtypestypes
#include "rdynarray.h"
#include "rfilemode.h"

namespace rtypes
{
    /* file_entry
     *  represents a filesystem entry; not every field may be supported
     */
    struct file_entry
    {
        file_entry();
        file_entry(const str& filePathName); // construct entry from specified item in filesystem [terr] [sys]

        str name; // file name
        timestamp createTime; // file create time
        timestamp changeTime; // file change time
        timestamp lastAccessTime; // last read time
        timestamp lastModifyTime; // last write time
        qword fileSize; // size of file in bytes
        dword fileMode; // system-specific bit-field containing file attributes

        file_entry_kind get_kind() const; // retrieves the type of filesystem entry based off 'fileMode' [sys]
    };

    typedef dynarray<file_entry> listing;
    typedef dynarray<str> simple_listing;

    /* path
     *  represents an actual or potential directory name and performs operations
     * on string representations of path names and on the corresponding
     * filesystem object
     */
    class path
    {
    public:
        path(); // construct empty path
        path(const str& pathName); // construct path from the specified string name (relative paths will be relative to the current working directory)
        path(const str& pathName,const str& relativeTo); // construct path from the specified string name, using the specified relative directory

        path& operator =(const str&); // set the path name [terr]
        path& operator +=(const str&); // append the specified name to the full path [terr]
        path& operator +=(const path&); // append differences to the full path [terr]

        bool exists() const; // returns true if the path exists [sys] [terr]
        bool is_empty() const; // if the directory has no children, returns true [sys] [terr]
        int count_elements() const; // counts the number of things in the path (files, links, directories) [sys]

        bool make(bool createSubDirectories = false) const; // create a directory derived from the path name with a default mode [sys] [lerr]
        bool copy(const path& name,bool moveContents = true,bool overwrite = false) const; // copy the directory and optionally its contents [lerr]
        bool copy(const str& name,bool moveContents = true,bool overwrite = false) const; // copy the directory and optionally its contents [lerr]
        bool rename(const path& name,bool keepNewName = true,bool overwrite = false); // rename the directory in the filesystem and optionally keep the new name [sys] [lerr]
        bool rename(const str& name,bool keepNewName = true,bool overwrite = false); // rename the directory in the filesystem and optionally keep the new name [sys] [lerr]
        bool remove(); // remove the directory derived from the path name but only if it is empty [sys] [lerr]
        bool erase(bool failOnSubDirectories = true); // remove the directory derived from the path name and all of its contents [sys] [lerr]

        bool get_complete_listing(listing&) const; // gets a listing of both files and sub-directories [sys] [lerr]
        bool get_complete_listing(simple_listing&) const; // returns true if the directory was successfully opened
        bool get_subdirectory_listing(listing&) const; // gets a listing of only sub-directories [sys] [lerr]
        bool get_subdirectory_listing(simple_listing&) const; // returns true if the directory was successfully opened
        bool get_file_listing(listing&) const; // gets a listing of only files [sys] [lerr]
        bool get_file_listing(simple_listing&) const; // returns true if the directory was successfully opened

        str get_name() const; // gets the name as-reported
        str get_top_name() const; // gets top-level directory name in path
        str get_full_name() const; // gets the fully qualified path
        str get_relative_name() const; // gets the path relative to the current working directory
        str get_relative_name(const path&) const; // gets the path relative to the specified directory
        str get_parent_name() const; // gets the leading path

        void set_name(const str& pathName); // set the relative name of the path (relative to the object's current absolute part) [terr]
        void set_name(const str& pathName,const str& relativeTo); // set the name of the path relative to another path [terr]
        void set_full_name(const str&); // sets the fully qualified path; the relative part is set to top directory [terr]
        void set_top_name(const str&); // sets the top-level directory name for the path [terr]
        void set_parent_path(const path&); // sets the leading path
        void set_relative_path(); // sets the path relative to the current working directory
        void set_relative_path(const path&); // sets the path relative to the specified directory

        file_entry get_entry_info() const; // retrieves filsytem entry-information for the directory [terr]
        bool set_directory_mode(dword modeBits); // sets the file mode of the directory with a system-independent bitmask of 'file_attribute' [sys] [lerr]

        void append_name(const str&); // append the specified name to the end of the top-level directory
        void append_before(const str&); // append the specified name before the top-level directory
        static path get_working_path(); // retrieve the current working directory [terr]
        static void set_working_path(const path&); // set the current working directory [sys] [terr]
    private:
        str _parts[2];

        void _checkParts(); // [sys] [terr]
        static str _getWorkDir(); // [sys] [terr]
        static str _getRel(const str&,const str&); // [sys]
    };

    /* filename
     *  represents a regular (data-containing) file, either actual or potential
     * within the filesystem; performs operations on the string representation
     * of a file name and on the corresponding filesystem object
     */
    class filename
    {
    public:
        filename();
        filename(const str& name);
        filename(const str& pathLocation,const str& name);
        filename(const path& pathLocation,const str& name);

        filename& operator =(const str&);

        bool exists() const; // indicates whether or not the file name exists in the filesystem [sys] [terr]
        bool has_extension() const; // indicates whether or not the file name object contains an extension
        bool has_contents() const; // indicates whether or not an existing file name's length is greater than zero [sys] [terr]
        qword size() const; // returns the size (in bytes) of an existing file [sys] [terr]

        bool create() const; // create the file if it does not exist in a default mode; returns true if the file was created OR was already created [sys] [lerr]
        bool copy(const path& toDirectory,bool overwrite = false) const; // copy file referred by this filename into new file of same name in specified path [lerr]
        bool copy(const str& toFile,bool overwrite = false) const; // copy file referred by this filename into new specified file [sys] [lerr]
        bool copy(const filename& toFile,bool overwrite = false) const; // copy file referred by this filename into new specified file [lerr]
        bool rename(const path& name,bool keepNewName = true,bool overwrite = false); // rename the file keeping the top level name the same but using the specified path [lerr]
        bool rename(const str& name,bool keepNewName = true,bool overwrite = false); // rename the file in the filesystem and optionally keep the new name [sys] [lerr]
        bool rename(const filename& name,bool keepNewName = true,bool overwrite = false); // rename the file in the filesystem and optionally keep the new name [lerr]
        bool deletef(); // removes the file from the filesystem [sys] [lerr]

        str get_name() const; // gets just the top level file name with any extension
        str get_namex() const; // gets the name of the file excluding any extension
        str get_extension(bool includeLeadingDot = false) const; // gets the extension; if the file has no extension returns empty string
        str get_full_name() const; // gets the fully qualified name of the file; includes the extension
        path& get_path(); // gets a reference to the path object that represents this file's path
        const path& get_path() const; // gets a reference to the path object that represents this file's path
        str get_relative_name() const; // gets the file location relative to the current directory
        str get_relative_name(const path&) const; // gets the file location relative to the specified directory

        void set_name(const str& name); // set the name and any extension
        void set_name(const str& name,const str& extension); // set the name and any extension
        void set_namex(const str& name); // set the name excluding any extension
        void set_extension(const str& name); // set only the extension part
        void set_full_name(const str& name); // set the fully qualified file name
        void set_path(const path&); // set just the path leading up to the file name

        file_entry get_entry_info() const; // gets a structure describing the file entry [terr]
        bool set_file_mode(dword modeBits); // sets the mode for the file using a system-independent bitmask of 'file_attribute' [sys]
    private:
        path _path;
        str _name;

        void _trunLeader(str&); // [sys]
    };

    // path/file name concatenation
    path operator +(const path&,const str&);
    filename operator +(const path&,const filename&);
}

#endif
