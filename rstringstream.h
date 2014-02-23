/* rstringstream.h
 *  rlibrary/rstringstream - provides implementations of rstream and rbinstream that operate
 * on a device of type 'str'; these streams implement the device scheme for
 * standard rstream/rbinstream derivations; output iterators are opened
 * for append by default
 */
#ifndef RSTRINGSTREAM_H
#define RSTRINGSTREAM_H
#include "rstream.h"
#include "rstreammanip.h" // each stream type makes good use of these

namespace rtypes
{
    class rstringstream : public rstream, public stream_device<str,generic_string>
    {
    public:
        rstringstream();
        rstringstream(const char* initialValue);
        rstringstream(generic_string& strDevice);
        ~rstringstream();
    private:
        virtual void _clearDevice();
        virtual bool _openDevice(const char*);
        virtual void _closeDevice();
                
        virtual bool _inDevice() const;
        virtual void _outDevice();
    };

    class const_rstringstream : public rstream, public const_stream_device<str,generic_string>
    {
    public:
        const_rstringstream();
        const_rstringstream(const char* initialValue);
        const_rstringstream(const generic_string& strDevice);
    private:
        virtual bool _openDevice(generic_string*,const char*);
        virtual void _closeDevice();
        
        virtual bool _inDevice() const;
        virtual void _outDevice() {}
    };
        
    class rbinstringstream : public rbinstream, public stream_device<str,generic_string>
    {
    public:
        rbinstringstream();
        rbinstringstream(const char* initialValue);
        rbinstringstream(generic_string& strDevice);
        rbinstringstream(endianness endianFlag);
        ~rbinstringstream();
    private:
        virtual void _clearDevice();
        virtual bool _openDevice(const char*);
        virtual void _closeDevice();
                
        virtual bool _inDevice() const;
        virtual void _outDevice();
    };

    class const_rbinstringstream : public rbinstream, public const_stream_device<str,generic_string>
    {
    public:
        const_rbinstringstream();
        const_rbinstringstream(const char* initialValue);
        const_rbinstringstream(const generic_string& strDevice);
        const_rbinstringstream(endianness endianFlag);
    private:
        virtual bool _openDevice(generic_string*,const char*);
        virtual void _closeDevice();

        virtual bool _inDevice() const;
        virtual void _outDevice() {}
    };
}

#endif
