//rstring.h - provides basic string types for rlibrary
#ifndef RSTRING_H
#define RSTRING_H
#include "rtypestypes.h"

#define RSTRING_DEFAULT_ALLOCATION 16

namespace rtypes
{
    /* rtype_string
     *  provides abstract interface and implementation for rlibrary strings
     */
    template<typename CharType>
    class rtype_string
    {
    public:
        rtype_string();
        virtual ~rtype_string();

        // unchecked access
        CharType& operator [](size_type index)
        { return _access(index); }
        const CharType& operator [](size_type index) const
        { return _buffer->data[index]; }

        // range checked access
        CharType& at(size_type index);
        const CharType& at(size_type index) const;

        // operators (invoked by derived class versions)
        rtype_string& operator =(CharType);
        rtype_string& operator =(const CharType*);
        rtype_string& operator =(const rtype_string&);
        rtype_string& operator +=(CharType);
        rtype_string& operator +=(const CharType* cStr);
        rtype_string& operator +=(const rtype_string&);

        // operations
        void append(CharType);
        void append(const CharType*);
        void append(const rtype_string&);
        void push_back(CharType);
        void clear(); // reduce the logical allocation size
        void reset(); // reduce the actual allocation size
        bool truncate(size_type desiredSize);
        void resize(size_type NewSize);

        // string info
        const CharType* c_str() const
        { return _buffer->data; }
        size_type size() const
        { return _buffer->size-1; }
        size_type length() const
        { return _buffer->size-1; }
        size_type capacity() const
        { return _buffer->size+_buffer->extra-1; }
        size_type allocation_size() const
        { return _buffer->size+_buffer->extra; }
    protected:
        struct _StringBuffer
        {
            _StringBuffer();
            virtual ~_StringBuffer();

            CharType* data;
            size_type size;
            size_type extra;

            void allocate(size_type desiredSize);
            void deallocate();
        private:
            // disallow copying
            _StringBuffer(const _StringBuffer&);
            _StringBuffer& operator =(const _StringBuffer&);
        };

        // string buffer
        _StringBuffer* _buffer;

        // virtual string interface
        virtual void _allocate(size_type desiredSize) = 0;
        virtual void _deallocate() = 0;
        virtual CharType& _access(size_type i)
        { return _buffer->data[i]; }

        void _copy(const CharType*);
        void _copy(const CharType*,size_type);
        void _nullTerm();

        static const _StringBuffer* _getBuffer(const rtype_string& object)
        { return object._buffer; }
    };

    /* deep_string
     *  represents a string that performs a deep copy of its internal string buffer; this
     * means that the string by default implements value semantics under normal usage;
     * these strings typically are more efficient and use less memory than their shallow string
     * counterparts; deep strings are used exclusively in the implementation of rlibrary, however
     * base class (rtype_string) references are used in read expressions (e.g. function parameters)
     * to maintain compatibility with shallow string types
     */
    template<typename CharType>
    class deep_string : public rtype_string<CharType>
    {
        typedef rtype_string<CharType> _Base;
    public:
        deep_string();
        deep_string(const CharType*);
        explicit deep_string(const _Base&);
        deep_string(const deep_string&);
        explicit deep_string(size_type allocSize);

        // these simply invoke the base class versions
        deep_string& operator =(CharType);
        deep_string& operator =(const CharType*);
        deep_string& operator =(const _Base&);
        deep_string& operator =(const deep_string&); // must overload for derived class type
        deep_string& operator +=(CharType);
        deep_string& operator +=(const CharType* cStr);
        deep_string& operator +=(const _Base&);
        deep_string& operator +=(const deep_string&); // must overload for derived class type
    protected:
        using _Base::_buffer;
        using _Base::_copy;
        using _Base::_nullTerm;
        using _Base::_getBuffer;
    private:
        virtual void _allocate(size_type desiredSize);
        virtual void _deallocate();

        typename _Base::_StringBuffer _buf;
    };

    /* shallow_string
     *  represents a string that performs a shallow copy of its internal buffer
     * and a deep copy on write (copy on write); this string type is available
     * for use in cases where default reference semantics are required/desired;
     * rlibrary does not typically provide such strings in its public interface
     * or implementation
     */
    template<typename CharType>
    class shallow_string : public rtype_string<CharType>
    {
        typedef rtype_string<CharType> _Base;
    public:
        shallow_string();
        shallow_string(const CharType*);
        explicit shallow_string(const _Base&);
        shallow_string(const shallow_string&);
        explicit shallow_string(size_type allocSize);
        ~shallow_string();

        // these simply invoke the base class versions
        // along with performing buffer checks for shallow
        // copies
        shallow_string& operator =(CharType);
        shallow_string& operator =(const CharType*);
        shallow_string& operator =(const _Base&);
        shallow_string& operator =(const shallow_string&); // must overload for derived class type
        shallow_string& operator +=(CharType);
        shallow_string& operator +=(const CharType* cStr);
        shallow_string& operator +=(const _Base&);
        shallow_string& operator +=(const shallow_string&); // must overload for derived class type
    protected:
        using _Base::_buffer;
        using _Base::_copy;
        using _Base::_nullTerm;
        using _Base::_getBuffer;
    private:
        struct _StringBufferEx : _Base::_StringBuffer
        {
            _StringBufferEx();

            int reference;
        };

        virtual void _allocate(size_type desiredSize);
        virtual void _deallocate();
        virtual CharType& _access(size_type);
    };

    // comparison operators
    template<typename CharType>
    bool operator ==(const rtype_string<CharType>&,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator ==(const rtype_string<CharType>&,const CharType*);
    template<typename CharType>
    bool operator ==(const CharType*,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator !=(const rtype_string<CharType>&,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator !=(const rtype_string<CharType>&,const CharType*);
    template<typename CharType>
    bool operator !=(const CharType*,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator <(const rtype_string<CharType>&,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator <(const rtype_string<CharType>&,const CharType*);
    template<typename CharType>
    bool operator <(const CharType*,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator >=(const rtype_string<CharType>&,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator >=(const rtype_string<CharType>&,const CharType*);
    template<typename CharType>
    bool operator >=(const CharType*,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator >(const rtype_string<CharType>&,const rtype_string<CharType>&);

    template<typename CharType>
    bool operator >(const rtype_string<CharType>&,const CharType*);

    template<typename CharType>
    bool operator >(const CharType*,const rtype_string<CharType>&);

    template<typename CharType>
    bool operator <=(const rtype_string<CharType>&,const rtype_string<CharType>&);
    template<typename CharType>
    bool operator <=(const rtype_string<CharType>&,const CharType*);
    template<typename CharType>
    bool operator <=(const CharType*,const rtype_string<CharType>&);

    // concatenation operators for deep strings
    template<typename CharType>
    deep_string<CharType> operator +(const rtype_string<CharType>&,const rtype_string<CharType>&);
    template<typename CharType>
    deep_string<CharType> operator +(const rtype_string<CharType>&,const CharType*);
    template<typename CharType>
    deep_string<CharType> operator +(const CharType*,const rtype_string<CharType>&);

    /* typedefs for common deep-string string types (both normal and wide characters are supported);
       users are encouraged to use these instead of the shallow_string types, especially in multi-
       threaded code */
    typedef rtype_string<char> generic_string; // generic string types
    typedef rtype_string<wchar_t> generic_wstring;
    typedef deep_string<char> str; // original rlibrary standard string types
    typedef deep_string<wchar_t> wstr;
    typedef str string; // alternates for original standard string types
    typedef wstr wstring;

}

// include out-of-line implementation
#include "rstring.tcc"

#endif
