/* rfile.h
 *  contains declarations for types used to peform
 * IO operations on file objects
 */
#ifndef RFILE_H
#define RFILE_H
#include "riodevice.h" // gets rstream
#include "rfilemode.h"

namespace rtypes
{
    typedef qword offset_type;
    typedef long long int offset_seek_type;

    /* file
     *  represents an object that wraps file
     * input/output operations; this class is
     * intended to provide lower-level support
     */
    class file : public io_device
    {
    public:
        file();
        file(const char* fileName,file_open_mode mode = file_open_always);

        // open a file for specific access and with specified disposition
        bool open(const char* fileName,file_open_mode mode = file_open_always);
        bool open_input(const char* fileName,file_open_mode mode = file_open_always);
        bool open_output(const char* fileName,file_open_mode mode = file_open_always);

        offset_type get_file_pointer() const; // gets file pointer [sys] [terr]
        offset_type get_file_size() const; // gets the file size in number of bytes [sys] [terr]

        // (I don't recommend using this; test get_last_operation_status() for 'no_input';
        // this is useful if you don't want to read an if another thread/process may have modified
        // the file pointer)
        bool eof() const; // returns true if the file pointer is at the end-of-file [sys] [terr]

        bool set_file_pointer(offset_type pos); // sets file pointer; returns true if 'pos' was already within range of the file [sys] [terr]
        bool seek_file_pointer(offset_seek_type posDif); // seeks the file pointer by the specified amount; returns false if the position was incorrect [sys] [terr]

        bool truncate(); // sets the file size to zero bytes; assume success [sys] [terr]
        bool resize(offset_type size); // sets the file size to the specified size; returns false if the operation couldn't complete [sys] [terr]
    private:
        virtual void _openEvent(const char* deviceID,
            io_access_flag accessKind,
            void** arguments = NULL,
            dword argumentCount = 0); // [sys]
        virtual void _readAll(generic_string& buffer) const; // [sys]
        virtual void _closeEvent(io_access_flag shutdownKind);
    };

    class file_stream_device : public stream_device<file>
    {
    protected:
        file_stream_device();
        file_stream_device(file& device);
    private:
        // stream device interface
        virtual void _clearDevice();
        virtual bool _openDevice(const char* deviceID);
        virtual void _closeDevice();
    };

    /* file_stream
     *  represents a text stream interface to a file; a 
     * file_stream (by default) buffers its output until it
     * is flushed, closed, destroyed, or until it encounters
     * the 'endline' stream manipulator; this class is intended
     * to provide in general higher-level support
     *  a file_stream locally buffers its output
     */
    class file_stream : public rstream,
                        public file_stream_device
    {
    public:
        file_stream();
        file_stream(const char* fileName);
        file_stream(file& fileDevice);
        ~file_stream();
    private:
        // stream buffer interface
        virtual bool _inDevice() const; // [sys]
        virtual void _outDevice(); // [sys]
    };

    /* file_binary_stream
     *  represents a binary stream interface to a file;
     * a file_binary_stream (by default) buffers its output until it
     * is flushed, closed, destroyed, or until it encounters the 'endline'
     * stream manipulator; this class is intended to provide in general
     * higher-level support
     *  a file_binary_stream locally buffers its output
     */
    class file_binary_stream : public rbinstream,
                               public file_stream_device
    {
    public:
        file_binary_stream();
        file_binary_stream(const char* fileName);
        file_binary_stream(file& fileDevice);
        ~file_binary_stream();
    private:
        // stream_buffer interface
        virtual bool _inDevice() const; // [sys]
        virtual void _outDevice(); // [sys]
    };
}

#endif
