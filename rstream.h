/* rstream.h
 *  rlibrary/rstream - provides basic functionality for text and binary streams
 */
#ifndef RSTREAM_H
#define RSTREAM_H
#include "rstring.h"
#include "rqueue.h"
#include "rset.h"
#include "rerror.h"

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
         * _peekInput( char&,size_type ) const
         * Gets the next zero-based indexed character from the input buffer; does not modify the state of the buffer
         */
        bool _peekInput(char&) const;
        bool _peekInput(char&,size_type) const;

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
        mutable size_type _ideviceIter;
        size_type _odeviceIter;
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
        stream_base(bool doesBuffer);
        virtual ~stream_base() {}

        char get();
        char peek() const;
        char peek(size_type) const; // peek zero-based indexed char from stream
        void put(char c);
        void place(const stream_base&); // place contents of specified stream's local output into this stream's local output buffer
        void repeat(char,uint32 times); // insert 'times' number of the specified character

        /* input iterators
         *  the input iterator is understood by the input device iter and the number of
         *  in the input buffer
         */
        size_type get_input_iter() const
        { return _ideviceIter-_bufIn.size(); }
        void seek_input_iter(ssize_type amount)
        { set_input_iter(get_input_iter()+amount); }
        void set_input_iter(size_type iter); // flushes input buffer
        void reset_input_iter()
        { set_input_iter(0); }

        size_type get_output_iter() const
        { return _odeviceIter; }
        void seek_output_iter(ssize_type amount)
        { set_output_iter(get_output_iter()+amount); }
        void set_output_iter(size_type iter); // flushes output buffer
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
    private:
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
        rstream(bool doesBuffer);
        virtual ~rstream() {}

        // delimit operations
        void add_extra_delimiter(char);
        void add_extra_delimiter(const char*);
        void add_extra_delimiter(const generic_string& delimiterString);
        void remove_extra_delimiter(char c);
        void remove_extra_delimiter(const char*);
        void remove_extra_delimiter(const generic_string& delimiterString);
        void clear_extra_delimiters();
        bool delimit_whitespace(bool yes);
        str get_last_delimited_space() const
        { return _delimStrLast; }
        str get_active_delimited_space() const
        { return _delimStrActive; }

        // stream manipulation
        uint16 width() const
        { return _width; }
        uint16 width(uint16 wide);
        byte precision() const
        { return _precision; }
        byte precision(byte ndigits);
        char fill() const
        { return _fill; }
        char fill(char fillChar);
        numeric_representation representation() const
        { return _repFlag; }
        numeric_representation representation(numeric_representation);

        // get string delimited by endline (does not include endline character(s))
        void getline(generic_string&);
        // put string followed by endline
        void putline(const generic_string&);

        // input operator overloads for basic types
        rstream& operator >>(bool&);
        rstream& operator >>(char&);
        rstream& operator >>(byte&);
        rstream& operator >>(short&);
        rstream& operator >>(uint16&);
        rstream& operator >>(int&);
        rstream& operator >>(uint32&);
        rstream& operator >>(long&);
        rstream& operator >>(unsigned long&);
        rstream& operator >>(int64&);
        rstream& operator >>(uint64&);
        rstream& operator >>(float&);
        rstream& operator >>(double&);
        rstream& operator >>(void*&);
        rstream& operator >>(generic_string&);

        // output operator overloads for basic types
        rstream& operator <<(bool);
        rstream& operator <<(char);
        rstream& operator <<(byte);
        rstream& operator <<(short);
        rstream& operator <<(uint16);
        rstream& operator <<(int);
        rstream& operator <<(uint32);
        rstream& operator <<(long);
        rstream& operator <<(unsigned long);
        rstream& operator <<(const int64&);
        rstream& operator <<(const uint64&);
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
        mutable str _delimStrActive, _delimStrLast;

        // manipulator fields
        uint16 _width;
        byte _precision;
        char _fill;
        numeric_representation _repFlag;

        void _init();
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

    enum binary_string_input_format
    {
        binary_string_capacity, // reads in a the capacity of a string object
        binary_string_null_terminated // reads until a null character is found
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
        rbinstream(bool doesBuffer); // set default endianness to little; set buffering mode
        rbinstream(endianness,bool doesBuffer); // set default endianness; set buffering mode

        //input operations
        rbinstream& operator >>(bool&);
        rbinstream& operator >>(char&);
        rbinstream& operator >>(byte&);
        rbinstream& operator >>(short&);
        rbinstream& operator >>(uint16&);
        rbinstream& operator >>(int&);
        rbinstream& operator >>(uint32&);
        rbinstream& operator >>(long&);
        rbinstream& operator >>(unsigned long&);
        rbinstream& operator >>(int64&);
        rbinstream& operator >>(uint64&);
        rbinstream& operator >>(double&);
        rbinstream& operator >>(void*&);
        rbinstream& operator >>(generic_string&);

        //output operations
        rbinstream& operator <<(bool);
        rbinstream& operator <<(char);
        rbinstream& operator <<(byte);
        rbinstream& operator <<(short);
        rbinstream& operator <<(uint16);
        rbinstream& operator <<(int);
        rbinstream& operator <<(uint32);
        rbinstream& operator <<(long);
        rbinstream& operator <<(unsigned long);
        rbinstream& operator <<(const int64&);
        rbinstream& operator <<(const uint64&);
        rbinstream& operator <<(const double&);
        rbinstream& operator <<(const void*);
        rbinstream& operator <<(const char*);
        rbinstream& operator <<(const generic_string&);
        rbinstream& operator <<(endianness);
        rbinstream& operator <<(binary_string_input_format);
    private:
        endianness _endianFlag;
        binary_string_input_format _stringInputFormat;

        template<class Numeric>
        void _pushBackBinaryOrder(Numeric);
        template<class Numeric>
        Numeric _fromString(const str&,bool&);
    };

    /* generic_stream_device and stream_device
     *  Abstract base classes that optionally can be inherited alongside
     * rstream or rbinstream that provide device functionality. A standard
     * rstream or rbinstream inherits one of these interfaces.
     *
     * A generic_stream_device<T> is an interface to some IO device of type T. It provides operations
     * for opening, closing, and maintaining the IO device. A generic_stream_device<T> refers to some
     * external IO device of type T or none at all. It merely stores a reference to a device. Initially,
     * this object is invalid since it refers to no device object.
     *
     * A stream_device<T,U> is a derivation of generic_stream_device<U> that provides operations related to
     * opening, closing, and maintaining some IO device of type U. A stream_device<T,U> always maintains a valid
     * reference to an object of type U. It operates in two modes: owned and un-owned.
     * Owned-mode: in this mode, the stream operates on a device IO object that it has allocated of type T; T should
     * be a sub-class of type U; this allows you to assign references to base class objects while still allowing the stream
     * device to instantiate a valid device object
     * Non-owned mode: in this mode, the stream operates on a device IO object that has been provided
     * for it by the user, in much the same way a generic_stream_device<U> object functions
     *
     * Any stream stream object has access to the rstream or rbinstream's stream buffer and iterators but nothing more.
     */
    template<typename DeviceT>
    class generic_stream_device : virtual protected stream_buffer
    {
    public:
        generic_stream_device()
            : _device(NULL)
        {
        }
        generic_stream_device(DeviceT& device)
            : _device(&device)
        {
        }
        generic_stream_device(const generic_stream_device& obj)
            : _device(obj._device)
        {
        }

        generic_stream_device& operator =(generic_stream_device& obj)
        {
            if (this != &obj)
            {
                // reset the state of the stream device
                forget();
                // assign reference
                _device = obj._device;
            }
            return *this;
        }

        void assign(DeviceT& device) // note: no return status is necessary since the device is managed in another context
        {
            // assign a reference to the specified device
            _assignDeviceAction(); // take care of anything that needs to be done beforehand
            _device = &device;
        }

        bool open(const char* deviceID)
        {
            // attempt open operation on device
            if (_device != NULL)
                return _openDevice(deviceID);
            return false;
        }

        void clear()
        {
            if (_device != NULL)
                _clearDevice();
            // reset iterators for accurate reporting
            _ideviceIter = 0;
            _odeviceIter = 0;
            // clear stream IO buffers
            _flushInputBuffer();
            _flushOutputBuffer();
        }

        void forget()
        {
            // almost close the device by invalidating the current reference
            if (_device != NULL)
            {
                // clear stream IO buffers
                _outDevice();
                _ideviceIter = 0;
                _odeviceIter = 0;
                _flushInputBuffer();
                _flushOutputBuffer();
                // let derived implementation perform forget action
                _forgetDeviceAction();
            }
        }

        void close()
        {
            if (_device != NULL)
            {
                // clear stream IO buffers
                _outDevice(); // route any data left over in the output buffer
                _ideviceIter = 0;
                _odeviceIter = 0;
                _flushInputBuffer();
                _flushOutputBuffer(); // just in case
                // let derived implementation close the device in some way
                _closeDevice();
                // let derived implementation perform close action
                _closeDeviceAction();
                // raise close event for derived implementation
                _closeEvent();
            }
        }

        DeviceT& get_device()
        { return *_device; }
        const DeviceT& get_device() const
        { return *_device; }
    protected:
        DeviceT* _device;
    private:
        /* virtual interface for immediate stream_device derivation */
        virtual void _assignDeviceAction()
        {
            // no action required
        }
        virtual void _forgetDeviceAction()
        {
            _device = NULL;
        }
        virtual void _closeDeviceAction()
        {
            _device = NULL;
        }

        /* virtual interface to be completed by derived class
         *      (any non-pure-virtual function is optional and by default does nothing)
         *
         * - _clearDevice(): should remove all data from the device if possible; if not possible
         *  this function should do nothing
         *
         * - _openDevice(): should open an owned device based on a string device ID
         *
         * - _closeDevice(): should perform implementation-defined operations to close the IO device
         *      and release any OS resources they were using. From the stream's perspective, it does
         *      not know how to properly close an IO device that it manages. Class stream_device<T> assumes
         *      that when an IO device reference is destroyed that the proper close action takes place.
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

    template<typename T,typename BaseT = T>
    class stream_device : public generic_stream_device<BaseT>
    {
    public:
        stream_device()
            : generic_stream_device<BaseT>(*(new T)), _owned(true)
        {
        }
        stream_device(BaseT& device)
            : generic_stream_device<BaseT>(device), _owned(false)
        {
        }
        stream_device(const stream_device& obj)
            : generic_stream_device<BaseT>(obj)
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
                // clear the stream buffer's state
                generic_stream_device<BaseT>::forget();
                // based on the owned state of 'obj', perform an assignment of the device
                if (obj._owned)
                {
                    if (_owned)
                        *_device = *obj._device;
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
    protected:
        using generic_stream_device<BaseT>::_device;

        bool _isOwned()
        { return _owned; }
    private:
        bool _owned;

        virtual void _assignDeviceAction() // called upon assigning a non-owned device (reference)
        {
            // if the device is owned, it needs to be closed
            // and deallocated
            if (_owned)
                delete _device; // assume destructor will close device
            _owned = false;
        }
        virtual void _forgetDeviceAction()
        {
            if (!_owned)
            {
                _device = new T; // forget non-owned reference and allocate new device object
                _owned = true;
            }
        }        
        virtual void _closeDeviceAction()
        {
            if (!_owned)
            {
                _device = new T; // forget non-owned reference and allocate new device object
                _owned = true;
            }
        }
    };

    /* const_stream_device
     *  Abstract base class that optionally can be inherited alongside
     * rstream or rbinstream that provides device functionality. A standard
     * conforming rstream or rbinstream should inherit this interface as well.
     *
     * A const_stream_device<T> is an interface to some IO device of type T. It provides operations
     * for opening, closing, and maintaining the IO device. A const_stream_device refers to a device object
     * only by reference. Unlike stream_device, it never creates a default device object. This object reference
     * is const-qualified, meaning all write operations have been restricted and its internal state cannot change
     * via the stream.
     *
     * A const_stream_device operates in two modes: valid and invalid depending on how the device was opened -
     * Invalid mode: in this mode, the stream does not operate on a device and any IO operations fail.
     * Valid mode: in this mode, the stream operates on an IO device object that has been provided
     * for it by the user.
     *
     * A const_stream_device<T> may have a non-const open operation. In anticipation of this, the interface member
     * function _openDevice accepts a non-const pointer to the const_stream_device<T>'s device object. This way, the
     * stream device can perform a non-const open operation. (This is implemented merely has a const_cast from _device.)
     *
     * A const_stream_device<T> has access to the rstream or rbinstream's stream buffer but nothing more and
     * may only read out of that buffer. If an rstream or rbinstream is using a const_stream_device, then the well-defined
     * behavior is to implement _outDevice as taking no action.
     */
    template<typename DeviceT>
    class const_stream_device : virtual protected stream_buffer
    {
    public:
        const_stream_device()
            : _device(NULL) { }
        const_stream_device(const DeviceT& device)
            : _device(&device) { }
        const_stream_device(const const_stream_device& obj)
        {
            // copy the reference (it may be invalid)
            _device = obj._device;
        }

        const_stream_device& operator =(const const_stream_device& obj)
        {
            if (this != &obj)
            {
                if (obj._device == NULL)
                    forget(); // match the null state of 'obj'
                else
                    assign(*obj._device);
            }
            return *this;
        }

        /* make this const stream device refer to the specified device
         * object; no open operation is attempted; the device may be in
         * whatever state
         */
        void assign(const DeviceT& device)
        {
            // reset stream in preparation for new device reference
            forget();
            // copy a reference to the device object
            _device = &device;
        }

        /* attempts an open operation on an assigned device; the operation
         * is left up to the virtual derived implementation
         */
        bool open(const char* deviceID)
        {
            if (_device != NULL)
                return _openDevice(const_cast<DeviceT*>(_device),deviceID);
            return false;
        }

        /* for a const stream device, this member
         * has a slightly different meaning; it
         * merely clears the stream buffers, not
         * the underlying device (since it's const)
         */
        void clear()
        {
            // reset input iterator (it's the only one that's used)
            _ideviceIter = 0;
            // flush input stream buffer
            _flushInputBuffer();
        }

        /* attempts to forget the current reference; the input state
         * of the stream is reset and the const_stream_device enters
         * an invalid state
         */
        void forget()
        {
            if (_device != NULL)
            {
                // clear the state of this device
                _ideviceIter = 0;
                _flushInputBuffer();
                // reset this state to null
                _device = NULL;
            }
        }

        /* attempts a close operation on the opened device, flushing
         * all buffers used for IO operations; the device object is
         * then forgotten and the const_stream_device enters an invalid
         * state
         */
        void close()
        {
            if (_device != NULL)
            {
                // clear the state of this device
                _ideviceIter = 0;
                _flushInputBuffer();
                // attempt to close device in some device
                // independent way
                _closeDevice();
                // raise close event that lets derived
                // implementation know that the device
                // was closed by the stream
                _closeEvent();
                // reset this state to null
                _device = NULL;
            }
        }

        bool is_valid()
        { return _device != NULL; }

        /* gets a reference to the current device
         */
        const DeviceT& get_device() const
        {
            if (_device != NULL)
                return *_device;
            throw invalid_operation_error();
        }
    protected:
        const DeviceT* _device;
    private:
        /* virtual interface to be completed by derived class
         *  (any non-pure-virtual function is optional and by default does nothing)
         * - _openDevice(): should open a valid device based on a string device ID; a pointer to
         *      a mutable device object is passed so that the open procedure can be completed
         *
         * - _closeDevice(): should perform implementation-defined operations to close the IO device
         *      and release any OS resources they were using; this member function is called when the close
         *      public interface function is called. From the stream's perspective, it does not know how to properly close
         *      an IO device that it owns and needs a reference.
         *
         * - _closeEvent() [optional]: should perform any close actions for the stream; _closeEvent is invoked each time
         *  a stream is being closed. This allows the stream's derived implementation to perform any needed close action.
         *  The default behavior of this member function is to do nothing.
         */
        virtual bool _openDevice(DeviceT* mutableDevice,const char* deviceID) = 0;
        virtual void _closeDevice() = 0;
        virtual void _closeEvent() {}
    };
}

#endif
