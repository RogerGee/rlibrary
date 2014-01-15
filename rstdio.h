/* rstdio.h
 *  contains declarations for types used for performing
 * input/output operations on the standard device channels
 */
#ifndef RSTDIO_H
#define RSTDIO_H
#include "riodevice.h" // gets rstream.h
#include "rstreammanip.h" // need these stream manipulators for standard_stream

namespace rtypes
{
    /* standard_device
     *  represents objects that wrap standard input/output
     * operations on the process's standard IO channels
     */
    class standard_device : public io_device
    {
    public:
        standard_device(); // opens the standard input, output, and error
        standard_device(io_access_flag desiredAccess); // opens the device for the specified access
        standard_device(const standard_device&); // creates a device based on the specified
        ~standard_device();

        standard_device& operator =(const standard_device&);

        // error IO operations
        void write_error(const generic_string& buffer) // writes the size of the specified string buffer to the device's error context
        { _writeErrBuffer(buffer.c_str(),buffer.size()); }
        void write_error(const void* buffer,dword length) // writes the specified buffer to the device's error context
        { _writeErrBuffer(buffer,length); }

        // error functionality
        bool open_error(const char* deviceID = NULL); // opens only the standard error device [sys]
        void redirect(const standard_device&); // saves the current IO context (if any) and applies any current, valid context from the specified device
        void redirect_error(const standard_device&); // saves the current error context (if any) and applies any current, valid error context from the specified device
        bool unredirect(); // closes the current IO context and loads the previous IO context (if any) (non-virtual override for standard_device type)
        bool unredirect_error(); // closes the current error context and loads the previous error context (if any)
        void close_error(); // closes the current and all previous error contexts
        bool is_redirected_error() const;
        bool is_valid_context() const // non-virtual override for standard_device type
        { return static_cast<const io_device&>(*this).is_valid_context() || is_valid_error(); }
        bool is_valid_error() const;

        // other functionality
        bool clear_screen(); // attempts to clear the screen if the device is a console/terminal; returns false if the operation could not be performed [sys]
    protected:
        io_resource* _error;

        virtual void _writeErrBuffer(const void* buffer,dword length); // write buffer to standard-error channel [sys]
    private:
        stack<io_resource*> _redirError;

        virtual void _openEvent(const char*,io_access_flag,dword**,dword);
        virtual void _readAll(generic_string&) const;
        virtual void _closeEvent(io_access_flag);
    };

    /* standard_stream
     *  represents a text stream interface to a standard device;
     * a standard_stream (by default) buffers its output until it
     * is flushed, closed, destroyed, or encounters the 'endline'
     * stream manipulator
     */
    class standard_stream : public rstream,
                            public stream_device<standard_device>
    {
    public:
        standard_stream();
        standard_stream(standard_device&);
        ~standard_stream();
    private:
        // stream device interface
        virtual void _clearDevice();
        virtual bool _openDevice(const char* deviceID);
        virtual void _closeDevice();

        // rstream interface
        virtual bool _inDevice() const; // [sys]
        virtual void _outDevice(); // [sys]
    };

    /* standard_binary_stream
     *  represents a binary stream interface to a standard device;
     * a standard_binary_stream (by default) buffers its output until it
     * is flushed, closes, destroyed, or encounters the 'endline'
     * stream manipulator
     */
    class standard_binary_stream : public rbinstream,
                                   public stream_device<standard_device>
    {
    public:
        standard_binary_stream();
        standard_binary_stream(standard_device&);
        ~standard_binary_stream();
    private:
        // stream device interface
        virtual void _clearDevice();
        virtual bool _openDevice(const char* deviceID);
        virtual void _closeDevice();

        // rstream interface
        virtual bool _inDevice() const; // [sys]
        virtual void _outDevice(); // [sys]
    };

    // standard stream object
    extern standard_stream stdConsole;
}

#endif
