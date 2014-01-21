/* rstream.h
 *  rlibrary/rstream - provides basic functionality for text and binary streams
 */
#ifndef RSTREAM_H
#define RSTREAM_H
#include "rstring.h"
#include "rqueue.h"
#include "rset.h"

namespace rtypes
{
    /* newline
     *  This library provides the constant 'newline' that provides
     * the standard endline representation for streams. An underlying
     * device may translate endline into its own native encoding, but must
     * make sure to translate to endline when putting data into a stream.
     */
    extern const char newline;

    /* stream_buffer
     *  A stream_buffer represents the buffer that performs IO transactions between
     * device and stream. The virtual interface _inDevice() and _outDevice() must be
     * completed in order for the buffer to fully function. Stream components rely
     * on the _popInput and _peekInput methods to get input from the stream input buffer.
     */
    class stream_buffer
    {
        typedef queue<char> buffer;
    protected:
        stream_buffer();

        mutable buffer _bufIn; // the state of the input buffer can change on an immutable operation
        buffer _bufOut; // the state of the output buffer cannot change on an immutable operation

        /* _popInput( char& )
         * Gets the next character from the input buffer and removes it from the buffer.
         */
        bool _popInput(char&);
        /* _peekInput( char& ) const
         * Gets the next character from the input buffer but does not modify the state of the buffer.
         * _peekInput( char&,dword ) const
         * Gets the next zero-based indexed character from the input buffer; does not modify the state of the buffer
         */
        bool _peekInput(char&) const;
        bool _peekInput(char&,dword) const;

        bool _hasInput() const
        { return !_bufIn.is_empty(); }

        void _flushInputBuffer()
        { _bufIn.clear(); }
        void _flushOutputBuffer()
        { _bufOut.clear(); }

        inline
        void _pushBackOutput(char c)
        { _bufOut.push(c); }
        /* Inserts the specified string into the output buffer
         */
        void _pushBackOutputString(const char*);
        void _pushBackOutputString(const generic_string&);

        /* _inDevice() const
         * This member function controls how input is read into the stream from the device;
         * it may only modify the state of _bufIn and cannot modify the state of _bufOut.
         * This member function should return false if no data was read from the device successfully.
         */
        virtual bool _inDevice() const = 0;

        /* _outDevice()
         * This member functions controls how output is written from the stream to the device;
         * this member function should write all data from the output buffer to the device (flush).
         * The buffer should be empty after the operation.
         */
        virtual void _outDevice() = 0;

        // these iterators are provided both for derived implementation
        // and the implementation of _outDevice and _inDevice
        mutable dword _ideviceIter;
        dword _odeviceIter;
    };

    /* stream_base
     *  Represents the base functionality for a text or binary rstream object. The base behavior
     * is get, peek, and put plus iteration, input success, output buffering:
     *      get - get the next character from the input stream and advance
     *      peek - get the next character from the input stream; don't advance
     *      put - place a character into the output stream
     *      iterator - controls the position from which bytes are read and to which they are written;
     *       iterator functionality provides an interface for the input/output iterator from stream_buffer;
     *       an iterator set operation always flushes the respective buffer because any data in the buffer
     *       is assumed to have been intended for the previous location
     */
    class stream_base : virtual protected stream_buffer
    {
    public:
        stream_base();
        virtual ~stream_base() {}

        char get();
        char peek() const;
        char peek(dword) const; // peek zero-based indexed char from stream
        void put(char c);
        void place(const stream_base&); // place contents of specified stream's local output into this stream's local output buffer
        void repeat(char,dword times); // insert 'times' number of the specified character

        /* input iterators
         *  the input iterator is understood by the input device iter and the number of
         *  in the input buffer
         */
        dword get_input_iter() const
        { return _ideviceIter-_bufIn.size(); }
        void seek_input_iter(int amount)
        { set_input_iter(get_input_iter()+amount); }
        void set_input_iter(dword iter); // flushes input buffer
        void reset_input_iter()
        { set_input_iter(0); }

        dword get_output_iter() const
        { return _odeviceIter; }
        void seek_output_iter(int amount)
        { set_output_iter(get_output_iter()+amount); }
        void set_output_iter(dword iter); // flushes output buffer
        void reset_output_iter()
        { set_output_iter(0); }

        bool get_input_success() const
        { return _lastSuccess; }
        void set_input_success(bool success)
        { _lastSuccess = success; }

        /* flush_output
         *  expose _outDevice as part of the interface since
         * flushing output is especially useful if buffering output
         */
        void flush_output()
        { _outDevice(); }

        /* buffered output
         *  note: these member functions do not flush
         * the output buffer; a next operation, destructor
         */
        void start_buffering_output()
        { _doesBuffer = true; }
        void stop_buffering_output()
        { _doesBuffer = false; }

        bool has_input() const;
        bool does_buffer_output() const
        { return _doesBuffer; }

        operator void*() const // determines the status of the last input operation
        { return (void*) _lastSuccess; }
    protected:
        mutable bool _lastSuccess;
        bool _doesBuffer; // applies to the output buffer; behavior enforced by derived implementation
    };

    /* forward declare rstream_manipulator
     */
    class rstream_manipulator;

    /* numeric_representation -
     *  represents a numeric base for numeric
     * to string conversions; any non-zero positive
     * base value may be converted to this type within
     * acceptable ranges
     */
    enum numeric_representation
    {
        binary = 2,
        octal = 8,
        decimal = 10,
        hexadecimal = 16
    };

    /* rstream
     *  represents a stream that provides a text interface to an underlying
     * stream buffer
     */
    class rstream : public stream_base
    {
    public:
        rstream();
        virtual ~rstream() {}

        // delimit operations
        void add_extra_delimiter(char);
        void add_extra_delimiter(const generic_string& delimiterString);
        void remove_extra_delimiter(char c);
        void remove_extra_delimiter(const generic_string& delimiterString);
        void clear_extra_delimiters();
        bool delimit_whitespace(bool yes);
        string get_last_delimited_space() const
        { return _delimStrLast; }
        string get_active_delimited_space() const
        { return _delimStrActive; }

        // stream manipulation
        word width() const
        { return _width; }
        word width(word wide);
        byte precision() const
        { return _precision; }
        byte precision(byte ndigits);
        char fill() const
        { return _fill; }
        char fill(char fillChar);

        // get string delimited by endline (does not include endline character(s))
        void getline(generic_string&);
        // put string followed by endline
        void putline(const generic_string&);

        // input operator overloads for basic types
        rstream& operator >>(bool&);
        rstream& operator >>(char&);
        rstream& operator >>(byte&);
        rstream& operator >>(short&);
        rstream& operator >>(word&);
        rstream& operator >>(int&);
        rstream& operator >>(dword&);
        rstream& operator >>(long&);
        rstream& operator >>(unsigned long&);
        rstream& operator >>(long long&);
        rstream& operator >>(qword&);
        rstream& operator >>(float&);
        rstream& operator >>(double&);
        rstream& operator >>(void*&);
        rstream& operator >>(generic_string&);

        // output operator overloads for basic types
        rstream& operator <<(bool);
        rstream& operator <<(char);
        rstream& operator <<(byte);
        rstream& operator <<(short);
        rstream& operator <<(word);
        rstream& operator <<(int);
        rstream& operator <<(dword);
        rstream& operator <<(long);
        rstream& operator <<(unsigned long);
        rstream& operator <<(const long long&);
        rstream& operator <<(const qword&);
        rstream& operator <<(float);
        rstream& operator <<(const double&);
        rstream& operator <<(const void*);
        rstream& operator <<(const char*);
        rstream& operator <<(const generic_string&);
        rstream& operator <<(numeric_representation);
        rstream& operator <<(const rstream_manipulator&);
    private:
        bool _delimitWhitespace; // determines if whitespace is used as a delimiter
        set<char> _delimits; // active delimiters not including whitespace
        mutable string _delimStrActive, _delimStrLast;

        // manipulator fields
        word _width;
        byte _precision;
        char _fill;
        numeric_representation _repFlag;

        bool _isWhitespace(char);

        template<typename Numeric>
        void _pushBackNumeric(Numeric,bool,const char* prefix = NULL);
        template<typename Numeric>
        Numeric _fromString(const str&,bool&);

        template<typename Numeric>
        str _convertNumeric(Numeric) const;

        template<typename Numeric>
        static Numeric _abs(Numeric&);
        template<typename Numeric>
        static Numeric _pow(Numeric,Numeric);
    };

    /* endianness
     *  represents the byte-order for a value
     * inserted into a binary stream
     */
    enum endianness
    {
        little, // least significant byte first
        big // most significant byte first
    };

    /* rbinstream
     *  represents a stream that provides a binary interface to
     * an underlying stream buffer
     */
    class rbinstream : public stream_base
    {
    public:
        rbinstream(); // set default endianness to little
        rbinstream(endianness); // set default endianness

        //input operations
        rbinstream& operator >>(bool&);
        rbinstream& operator >>(char&);
        rbinstream& operator >>(byte&);
        rbinstream& operator >>(short&);
        rbinstream& operator >>(word&);
        rbinstream& operator >>(int&);
        rbinstream& operator >>(dword&);
        rbinstream& operator >>(long&);
        rbinstream& operator >>(unsigned long&);
        rbinstream& operator >>(long long&);
        rbinstream& operator >>(qword&);
        rbinstream& operator >>(double&);
        rbinstream& operator >>(void*&);
        rbinstream& operator >>(generic_string&);

        //output operations
        rbinstream& operator <<(bool);
        rbinstream& operator <<(char);
        rbinstream& operator <<(byte);
        rbinstream& operator <<(short);
        rbinstream& operator <<(word);
        rbinstream& operator <<(int);
        rbinstream& operator <<(dword);
        rbinstream& operator <<(long);
        rbinstream& operator <<(unsigned long);
        rbinstream& operator <<(const long long&);
        rbinstream& operator <<(const qword&);
        rbinstream& operator <<(const double&);
        rbinstream& operator <<(const void*);
        rbinstream& operator <<(const char*);
        rbinstream& operator <<(const generic_string&);
        rbinstream& operator <<(endianness);
    private:
        endianness _endianFlag;

        template<class Numeric>
        void _pushBackBinaryOrder(Numeric);
        template<class Numeric>
        Numeric _fromString(const str&,bool&);
    };

    /* stream_device
     *  Abstract base class that optionally can be inherited alongside
     * rstream or rbinstream that provides device functionality. A standard
     * rstream or rbinstream must inherit this interface as well.
     *
     * A stream_device<T> is an interface to some IO device of type T. It provides operations
     * for opening, closing, and maintaining the IO device. A stream_device<T> always refers to some
     * valid object of type T, though IO on the object may be invalid. It operates in two modes: owned
     * and un-owned.
     * Owned-mode: in this mode, the stream operates on a device IO object that it has allocated
     * Non-owned mode: in this mode, the stream operates on a device IO object that has been provided
     * for it by the user
     *
     * A stream_device<T> has a second default template parameter which optionally can indicate the base
     * type to use, in case the device type is a sub-class from some heirarchy. This is useful if you 
     * want the stream device to handle any sub-class in unowned mode yet still be able to create a concrete
     * object of one of the sub-classes in the heirarchy.
     *
     * A stream_device<T> has access to the rstream or rbinstream's stream buffer but nothing more.
     */
    template<typename T,typename BaseT = T>
    class stream_device : virtual protected stream_buffer
    {
    public:
        stream_device()
        {
            _device = new T;
            _owned = true;
        }
        stream_device(const stream_device& obj)
        {
            if (obj._owned)
            {
                // make a copy of the device (deep copy)
                _device = new T( *obj._device );
                _owned = true;
            }
            else
            {
                // copy the reference
                _device = obj._device;
                _owned = false;
            }
        }
        virtual ~stream_device()
        {
            if (_owned)
                delete _device;
        }

        stream_device& operator =(const stream_device& obj)
        {
            if (this != &obj)
            {
                // clear IO buffers
                _outDevice();
                _flushInputBuffer();
                _flushOutputBuffer();
                if (obj._owned)
                {
                    if (_owned)
                    {
                        _closeDevice();
                        *_device = *obj._device;
                    }
                    else // forget old non-owned reference and create new owned device
                        _device = new T( *obj._device );
                }
                else
                {
                    // copy the reference
                    if (_owned)
                        delete _device; // assume that the destructor will close the device
                    _device = obj._device;
                }
                _owned = obj._owned;
            }
            return *this;
        }

        bool open(const char* deviceID)
        {
            // open an owned device
            if (_owned)
                return _openDevice(deviceID);
            return false;
        }
        void open(BaseT& device) // note: no return status is necessary since the device is managed in another context
        {
            // open a non-owned device
            if (_owned)
                delete _device; // assume destructor will close device
            _device = &device;
            _owned = false;
        }

        void clear()
        {
            _clearDevice();
            // reset iterators for accurate reporting
            _ideviceIter = 0;
            _odeviceIter = 0;
            // clear stream IO buffers
            _flushInputBuffer();
            _flushOutputBuffer();
        }

        void close()
        {
            // clear stream IO buffers
            _outDevice(); // route any data left over in the output buffer
            _flushInputBuffer();
            _flushOutputBuffer(); // just in case
            if (_owned) // if owned, simply close
                _closeDevice();
            else
            {
                _device = new T; // forget non-owned reference and allocate new device object
                _owned = true;
            }
            // raise close event that lets derived
            // implementation know that the device
            // was closed by the stream
            _closeEvent();
        }

        BaseT& get_device()
        { return *_device; }
        const BaseT& get_device() const
        { return *_device; }
    protected:
        BaseT* _device;

        bool _isOwned()
        { return _owned; }
    private:
        bool _owned;

        /* virtual interface to be completed by derived class
         *      (any non-pure-virtual function is optional and by default does nothing)
         *
         * - _clearDevice(): should remove all data from the device if possible; if not possible
         *  this function should do nothing
         *
         * - _openDevice(): should open an owned device based on a string device ID
         *
         * - _closeDevice(): should perform implementation-defined operations to close the IO device
         *      and release any OS resources they were using; this member function is only called when
         *      an owned device is being closed. From the stream's perspective, it does not know how to properly close
         *      an IO device that it owns and needs a reference. stream_device<T> assumes that when an IO device reference
         *      is destroyed that the proper close action takes place
         *
         * - _closeEvent() [optional]: should perform any close actions for the stream; unlike _closeDevice, _closeEvent
         *  is invoked each time a stream is being closed. This allows the stream's derived implementation to perform any
         *  needed close action. The default behavior of this member function is to do nothing.
         */
        virtual void _clearDevice() = 0;
        virtual bool _openDevice(const char* DeviceID) = 0;
        virtual void _closeDevice() = 0;
        virtual void _closeEvent() {}
    };

    /* const_stream_device
     *  Abstract base class that optionally can be inherited alongside
     * rstream or rbinstream that provides device functionality. A standard
     * rstream or rbinstream must inherit this interface as well.
     *
     * A const_stream_device<T> is an interface to some IO device of type T. It provides operations
     * for opening, closing, and maintaining the IO device. A const_stream_device<T> always refers to some
     * valid const object of type T, though IO on the object may be invalid. Output to the object is
     * prohibited since the object is const and its state cannot change. If the object has a non-const
     * input operation, then it does not conform to standard rstream/rbinstream functionality. A const_stream_device
     * operates in two modes: owned and non-owned depending on how the device was opened
     * Owned-mode: in this mode, the stream operates on a device IO object that it has allocated
     * Non-owned mode: in this mode, the stream operates on a device IO object that has been provided
     * for it by the user
     *
     * A stream_device<T> has a second default template parameter which optionally can indicate the base
     * type to use, in case the device type is a sub-class from some heirarchy. This is useful if you 
     * want the stream device to handle any sub-class in unowned mode yet still be able to create a concrete
     * object of one of the sub-classes in the heirarchy.
     *
     * A const_stream_device<T> may have a non-const open operation. In anticipation of this, the interface member
     * function _openDevice accepts a non-const pointer to the const_stream_device<T>'s device object. This way, the
     * stream device can perform a non-const open operation. (This is implemented merely has a const_cast from _device.)
     *
     * A const_stream_device<T> has access to the rstream or rbinstream's stream buffer but nothing more and
     * may only read into that buffer. If an rstream or rbinstream is using a const_stream_device, then the well-defined
     * behavior is to implement _outDevice as taking no action.
     */
    template<typename T,typename BaseT = T>
    class const_stream_device : virtual protected stream_buffer
    {
    public:
        const_stream_device()
        {
            _device = new T;
            _owned = true;
        }
        const_stream_device(const const_stream_device& obj)
        {
            if (obj._owned)
            {
                // make a copy of the device (deep copy)
                _device = new T( *obj._device );
                _owned = true;
            }
            else
            {
                // copy the reference
                _device = obj._device;
                _owned = false;
            }
        }
        virtual ~const_stream_device()
        {
            if (_owned)
                delete _device;
        }

        const_stream_device& operator =(const const_stream_device& obj)
        {
            if (this != &obj)
            {
                // clear input buffer
                _flushInputBuffer();
                if (obj._owned)
                {
                    if (_owned)
                    {
                        _closeDevice();
                        *const_cast<BaseT*>(_device) = *obj._device;
                    }
                    else // forget old non-owned reference and create new owned device
                        _device = new T( *obj._device );
                }
                else
                {
                    // copy the reference
                    if (_owned)
                        delete _device; // assume that the destructor will close the device
                    _device = obj._device;
                }
                _owned = obj._owned;
            }
            return *this;
        }

        bool open(const char* pDeviceID)
        {
            if (_owned)
                return _openDevice(const_cast<BaseT*>(_device),pDeviceID);
            return false;
        }
        void open(const BaseT& device)
        {
            // open a non-owned device
            if (_owned)
                delete _device; // assume destructor will close device
            _device = &device;
            _owned = false;
        }

        /* for a const stream device, this member
         * has a slightly different meaning; it
         * merely clears the stream buffers, not
         * the underlying device
         */
        void clear()
        {
            // reset input iterator (it's the only one that's used)
            _ideviceIter = 0;
            // flush stream IO buffers
            _flushInputBuffer();
            _flushOutputBuffer();
        }

        void close()
        {
            // clear stream IO buffers
            _flushInputBuffer();
            _flushOutputBuffer();
            if (_owned) // if owned, simply close
                _closeDevice();
            else
            {
                _device = new T; // forget non-owned reference and allocate new device object
                _owned = true;
            }
            // raise close event that lets derived
            // implementation know that the device
            // was closed by the stream
            _closeEvent();
        }

        const BaseT& get_device() const
        { return *_device; }
    protected:
        const BaseT* _device;

        bool _isOwned() const
        { return _owned; }
    private:
        bool _owned;

        /* virtual interface to be completed by derived class
         *  (any non-pure-virtual function is optional and by default does nothing)
         * - _openDevice(): should open an owned device based on a string device ID; a pointer to
         *      a mutable device object is passed so that the open procedure can be completed
         *
         * - _closeDevice(): should perform implementation-defined operations to close the IO device
         *      and release any OS resources they were using; this member function is only called when
         *      an owned device is being closed. From the stream's perspective, it does not know how to properly close
         *      an IO device that it owns and needs a reference. stream_device<T> assumes that when an IO device reference
         *      is destroyed that the proper close action takes place
         *
         * - _closeEvent() [optional]: should perform any close actions for the stream; unlike _closeDevice, _closeEvent
         *  is invoked each time a stream is being closed. This allows the stream's derived implementation to perform any
         *  needed close action. The default behavior of this member function is to do nothing.
         */
        virtual bool _openDevice(BaseT* mutableDevice,const char* deviceID) = 0;
        virtual void _closeDevice() = 0;
        virtual void _closeEvent() {}
    };
}

#endif
