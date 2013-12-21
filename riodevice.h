/* riodevice.h
 */
#ifndef RIODEVICE_H
#define RIODEVICE_H
#include "rresource.h" // gets rtypestypes.h
#include "rstring.h"
#include "rstack.h"

namespace rtypes
{
    /* io_operation_flag
     *  describes the exit status for an IO 
     * device operation
     */
    enum io_operation_flag
    {
        no_device,
        no_operation,
        success_read,
        success_write,
        no_input,
        no_output,
        bad_read,
        bad_write
    };

    /* io_access_flag
     *  describes the access for an IO device
     */
    enum io_access_flag
    {
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
        virtual ~io_device();

        bool open(const char* deviceID = NULL);
        bool open_input(const char* deviceID = NULL);
        bool open_output(const char* deviceID = NULL);

        void redirect(const io_device&);
        void redirect_input(const io_device&);
        void redirect_output(const io_device&);

        bool unredirect();
        bool unredirect_input();
        bool unredirect_output();

        void close();
        void close_input();
        void close_output();
    protected:

        io_resource* _input;
        io_resource* _output;
    private:
        stack<io_resource*> _redirInput;
        stack<io_resource*> _redirOutput;

        // virtual device interface
        virtual void _openEvent(const char* deviceID,io_access_flag accessKind,dword** arguments = NULL,dword argumentCount = 0) = 0;
        virtual void _closeEvent(io_access_flag shutdownKind) = 0;
        
    };
}

#endif
