/* rfilemode.h
 *  rlibrary/rfilemode - provides a cross-platform
 * mechanism for applying a mode to a file, either the file
 * on disk or a file resource in memory.
 */
#ifndef RFILEMODE_H
#define RFILEMODE_H

namespace rtypes
{
    /* file_entry_kind
     *  These values enumerate the kind
     * of possible files for an entry.
     */
    enum file_entry_kind
    {
        kind_regular_file,
        kind_directory,
        kind_link,
        kind_something_else
    };

    /* file_attribute
     *  These file attributes exist to provide
     * an interface to attributes for a file on disk.
     * They may or may not be used directly by
     * the underlying filesystem, that is, correspond
     * to actual file system attribute bits.
     */
    enum file_attribute
    {
        file_attribute_normal = 0x0000, // use default conventions for the file; overrides other attributes
        file_attribute_hidden = 0x0002, // use platform conventions for hidden files
        file_attribute_readonly = 0x0004 // use platform conventions to deny reading from a file
    };

    enum file_open_mode
    {
    };

    enum file_create_mode
    {
    };
}

#endif
