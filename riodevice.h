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

    class io_resource : public resource<8>
    {
        friend class io_device;
    public:
        io_resource();
        ~io_resource();

        void close(); // closes the IO resource in a system-specific manner [sys] [terr]
    private:
        int _reference;
    };

    class io_device
    {
    public:
        io_device();
        virtual ~io_device();

    protected:

        io_resource* _input;
        io_resource* _output;
    private:
        
    };
}

#endif
