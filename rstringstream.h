/* rstringstream.h
 *  rlibrary/rstringstream - provides implementations of rstream and rbinstream that operate
 * on a device of type 'str'; these streams implement the device scheme for
 * standard rstream/rbinstream derivations; output iterators are opened
 * for append by default
 */
#ifndef RSTRINGSTREAM_H
#define RSTRINGSTREAM_H
#include "rstream.h"

namespace rtypes
{
    class rstringstream : public rstream, public stream_device<str>
    {
    public:
        rstringstream();
        rstringstream(const char* initialValue);
        rstringstream(str& strDevice);
        ~rstringstream();
    private:
        virtual void _clearDevice();
        virtual bool _openDevice(const char*);
        virtual void _closeDevice();
                
        virtual bool _inDevice() const;
        virtual void _outDevice();
    };

    class const_rstringstream : public rstream, public const_stream_device<str>
    {
    public:
        const_rstringstream();
        const_rstringstream(const char* initialValue);
        const_rstringstream(const str& strDevice);
    private:
        virtual bool _openDevice(str*,const char*);
        virtual void _closeDevice();
        
        virtual bool _inDevice() const;
        virtual void _outDevice() {}
    };
        
    class rbinstringstream : public rbinstream, public stream_device<str>
    {
    public:
        rbinstringstream();
        rbinstringstream(const char* initialValue);
        rbinstringstream(str& strDevice);
        rbinstringstream(endianness endianFlag);
        ~rbinstringstream();
    private:
        virtual void _clearDevice();
        virtual bool _openDevice(const char*);
        virtual void _closeDevice();
                
        virtual bool _inDevice() const;
        virtual void _outDevice();
    };

    class const_rbinstringstream : public rbinstream, public const_stream_device<str>
    {
    public:
        const_rbinstringstream();
        const_rbinstringstream(const char* initialValue);
        const_rbinstringstream(const str& strDevice);
        const_rbinstringstream(endianness endianFlag);
    private:
        virtual bool _openDevice(str*,const char*);
        virtual void _closeDevice();

        virtual bool _inDevice() const;
        virtual void _outDevice() {}
    };
}

#endif
