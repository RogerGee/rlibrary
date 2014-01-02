/* rstdio.h
 *  contains declarations for types used for performing
 * input/output operations on the standard device channels
 */
#ifndef RSTDIO_H
#define RSTDIO_H
#include "riodevice.h" // gets rstream.h

namespace rtypes
{
    /* standard_device
     *  represents objects that wrap standard input/output
     * operations on the process's standard IO channels
     */
    class standard_device : public io_device
    {
    public:
        standard_device();
        standard_device(const standard_device&);
        ~standard_device();

        standard_device& operator =(const standard_device&);

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
    protected:
        io_resource* _error;

        virtual void _writeErrBuffer(const void* buffer,dword length); // write buffer to standard-error channel [sys]
    private:
        stack<io_resource*> _redirError;

        virtual void _openEvent(const char*,io_access_flag,dword**,dword);
        virtual void _readAll(generic_string&);
        virtual void _closeEvent(io_access_flag);
    };
}

#endif
