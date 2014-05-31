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
    class stringstream_io : public rstream
    {
    public:
        stringstream_io();
    protected:
        bool _inDeviceImpl(const generic_string* device) const;
        void _outDeviceImpl(generic_string* device);
    };
    class binstringstream_io : public rbinstream
    {
    public:
        binstringstream_io();
        binstringstream_io(endianness);
    protected:
        bool _inDeviceImpl(const generic_string* device) const;
        void _outDeviceImpl(generic_string* device);
    };

    class string_stream_device : public stream_device<str,generic_string>
    {
    public:
        string_stream_device() {}
        string_stream_device(generic_string& strDevice)
            : stream_device<str,generic_string>(strDevice) {}
    private:
        virtual void _clearDevice();
        virtual bool _openDevice(const char*);
        virtual void _closeDevice();
    };
    class generic_string_stream_device : public generic_stream_device<generic_string>
    {
    public:
        generic_string_stream_device() {}
        generic_string_stream_device(generic_string& strDevice)
            : generic_stream_device<generic_string>(strDevice) {}
    private:
        virtual void _clearDevice();
        virtual bool _openDevice(const char*);
        virtual void _closeDevice();
    };
    class const_string_stream_device : public const_stream_device<generic_string>
    {
    public:
        const_string_stream_device() {}
        const_string_stream_device(const generic_string& strDevice)
            : const_stream_device<generic_string>(strDevice) {}
    private:
        virtual bool _openDevice(generic_string*,const char*);
        virtual void _closeDevice();
    };

    class stringstream : public stringstream_io,
                         public string_stream_device
    {
    public:
        stringstream();
        stringstream(const char* initialValue);
        stringstream(generic_string& strDevice);
        ~stringstream();
    private:
        virtual bool _inDevice() const
        { return _inDeviceImpl(_device); }
        virtual void _outDevice()
        { _outDeviceImpl(_device); }
    };

    class ref_stringstream : public stringstream_io,
                             public generic_string_stream_device
    {
    public:
        ref_stringstream();
        ref_stringstream(generic_string& strDevice);
        ~ref_stringstream();
    private:
        virtual bool _inDevice() const
        { return _inDeviceImpl(_device); }
        virtual void _outDevice()
        { _outDeviceImpl(_device); }
    };

    class const_stringstream : public stringstream_io,
                               public const_string_stream_device
    {
    public:
        const_stringstream();
        const_stringstream(const generic_string& strDevice);
    private:
        virtual bool _inDevice() const
        { return _inDeviceImpl(_device); }
        virtual void _outDevice() {} // not allowed
    };

    class binstringstream : public binstringstream_io,
                            public string_stream_device
    {
    public:
        binstringstream();
        binstringstream(const char* initialValue);
        binstringstream(generic_string& strDevice);
        binstringstream(endianness endianFlag);
        ~binstringstream();
    private:
        virtual bool _inDevice() const
        { return _inDeviceImpl(_device); }
        virtual void _outDevice()
        { _outDeviceImpl(_device); }
    };

    class ref_binstringstream : public binstringstream_io,
                                public generic_string_stream_device
    {
    public:
        ref_binstringstream();
        ref_binstringstream(generic_string& strDevice);
        ref_binstringstream(endianness endianFlag);
        ~ref_binstringstream();
    private:
        virtual bool _inDevice() const
        { return _inDeviceImpl(_device); }
        virtual void _outDevice()
        { _outDeviceImpl(_device); }
    };

    class const_binstringstream : public binstringstream_io,
                                  public const_string_stream_device
    {
    public:
        const_binstringstream();
        const_binstringstream(const generic_string& strDevice);
        const_binstringstream(endianness endianFlag);
    private:
        virtual bool _inDevice() const
        { return _inDeviceImpl(_device); }
        virtual void _outDevice() {} // not allowed
    };
}

#endif
