/* riodevice.h
 *  contains base input/output functionality; this header should not 
 * have to be included directly
 */
#ifndef RIODEVICE_H
#define RIODEVICE_H
#include "rresource.h" // gets rtypestypes.h
#include "rstring.h"
#include "rstack.h"
#include "rstream.h"

namespace rtypes
{
    /* io_operation_flag
     *  describes the exit status for an IO 
     * device operation
     */
    enum io_operation_flag
    {
        no_device, // no valid IO context exists for the last operation
        no_operation, // an operation has not been performed on any valid IO context(s)
        success_read, // input operation was successful
        success_write, // output operation was successful
        no_input, // the input device did not return any input
        no_output, // the output device did not accept any output
        bad_read, // the input device failed to complete the operation
        bad_write // the output device failed to complete the operation
    };

    /* io_access_flag
     *  describes the access for an IO device
     */
    enum io_access_flag
    {
        no_access = 0x0000,
        write_access = 0x0001,
        read_access = 0x0002,
        all_access = 0x0003
    };

    /* io_resource
     *  describes a system IO resource in a cross-platform way;
     * 8 bytes are allocated for the resource identifier (handle,
     * descriptor, ETC.)
     */
    class io_resource : public resource<8>
    {
        friend class io_device;
        typedef resource<8> _MyBase;
    public:
        io_resource(); // initialize an invalid resource in a system-specific manner [sys]
        io_resource(void* defaultValue); // initialize a resource with the specified default value
        ~io_resource(); // closes the IO resource in a system-specific manner [sys] [terr]
    private:
        int _reference;
    };

    /* io_device
     *  an abstract base-class that represents an input-output device
     * that wraps a system IO resource
     */
    class io_device
    {
    public:
        io_device();
        io_device(const io_device&); // creates a copy of the current IO context of the specified device
        virtual ~io_device();

        io_device& operator =(const io_device&); // creates a copy of the current IO context of the specified device

        void read(str& buffer) // reads the capacity of the specified string object from the device
        { _readBuffer(&buffer[0],buffer.capacity()); }
        void read(void* buffer,dword bytesToRead) // reads the specified amount of bytes from the device into the specified buffer
        { _readBuffer(buffer,bytesToRead); }
        str read(dword bytesToRead = 0); // reads the specified number of bytes from the device into a string buffer and returns the result
        void write(const str& buffer) // writes the size of the specified string buffer to the device
        { _writeBuffer(buffer.c_str(),buffer.size()); }
        void write(const void* buffer,dword length) // writes the specified buffer to the device
        { _writeBuffer(buffer,length); }

        io_operation_flag get_last_operation_status() const // returns the last operation status flag
        { return _lastOp; }
        dword get_last_byte_count() const // returns the last number of bytes processed
        { return _byteCount; }

        bool open(const char* deviceID = NULL); // opens the device in some device-specific way using (if specified) a device string ID
        bool open_input(const char* deviceID = NULL); // opens only the input device in some device-specific way using (if specified) a device string ID
        bool open_output(const char* deviceID = NULL); // opens only the output device in some device-specific way using (if specified) a device string ID

        void redirect(const io_device&); // saves the current IO context (if any) and applies any current, valid context from the specified device
        void redirect_input(const io_device&); // saves the current input context (if any) and applies any current, valid input context from the specified device
        void redirect_output(const io_device&); // saves the current output context (if any) and applies any current, valid output context from the specified device

        bool unredirect(); // closes the current IO context and loads the previous IO context (if any)
        bool unredirect_input(); // closes the current input context and loads the previous input context (if any)
        bool unredirect_output(); // closes the current output context and loads the previous output context (if any)

        void close(); // closes the current and all previous IO contexts
        void close_input(); // closes the current and all previous input contexts
        void close_output(); // closes the current and all previous output contexts

        bool is_redirected_input() const; // determines if any previous input contexts are available
        bool is_redirected_output() const; // determines if any previous output contexts are available
        bool is_valid_context() const // determines if the current IO context is valid (at least one context is available)
        { return is_valid_input() || is_valid_output(); }
        bool is_valid_input() const; // determines if the current input context is available
        bool is_valid_output() const; // determines if the current output context is available
    protected:
        io_resource* _input;
        io_resource* _output;
        stack<io_resource*> _redirInput;
        stack<io_resource*> _redirOutput;
        io_operation_flag _lastOp;
        dword _byteCount;

        // generic read/write interface
        virtual void _readBuffer(void* buffer,dword bytesToRead); // reads a buffer from the device [sys]
        virtual void _writeBuffer(const void* buffer,dword length); // writes a buffer to the device [sys]

        static int& _ResourceRef(io_resource*);
    private:
        // abstract device interface
        virtual void _openEvent(const char* deviceID,
            io_access_flag accessKind,
            dword** arguments = NULL,
            dword argumentCount = 0) = 0; // opens a device in a system specific manner
        virtual void _readAll(str& buffer) = 0; // reads all available data from the device into the specified buffer, resizing as necessary
        virtual void _closeEvent(io_access_flag shutdownKind) = 0; // called whenever a device is completely shutdown
    };
}

#endif
