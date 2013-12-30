//rstring.h - provides basic string types for rlibrary
#ifndef RSTRING_H
#define RSTRING_H
#include "rtypestypes.h"

#define RSTRING_DEFAULT_ALLOCATION 16

namespace rtypes
{
    /* rtype_string
     *  provides abstract interface and
     * implementation for rlibrary strings
     */
    template<typename CharType>
    class rtype_string
    {
    public:
        rtype_string();
        virtual ~rtype_string();

        CharType& operator [](dword index);
        const CharType& operator [](dword index) const;

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
        bool truncate(dword desiredSize);
        void resize(dword NewSize);
        
        // string info
        const CharType* c_str() const 
        { return _buffer->data; }
        dword size() const 
        { return _buffer->size-1; }
        dword length() const 
        { return _buffer->size-1; }
        dword capacity() const 
        { return _buffer->size+_buffer->extra-1; }
        dword allocation_size() const 
        { return _buffer->size+_buffer->extra; }
    protected:
        struct _StringBuffer
        {
            _StringBuffer();
            virtual ~_StringBuffer();

            CharType* data;
            dword size;
            dword extra;

            void allocate(dword desiredSize);
            void deallocate();
        private:
            // disallow copying
            _StringBuffer(const _StringBuffer&);
            _StringBuffer& operator =(const _StringBuffer&);
        };

        // string buffer
        _StringBuffer* _buffer;

        // virtual string interface
        virtual void _allocate(dword desiredSize) = 0;
        virtual void _deallocate() = 0;
        virtual void _copy(const CharType*,dword) = 0;

        void _copy(const CharType*); // (calls _copy() overload)
        void _nullTerm();
    };

    /* deep_string
     *  represents a string that performs a deep
     * copy of its internal string buffer
     */
    template<typename CharType>
    class deep_string : public rtype_string<CharType>
    {
        typedef rtype_string<CharType> _Base;
    public:
        deep_string();
        deep_string(const CharType*);
        deep_string(const _Base&);
        deep_string(const deep_string&);
        explicit deep_string(dword allocSize);

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
    private:
        virtual void _allocate(dword desiredSize);
        virtual void _deallocate();
        virtual void _copy(const CharType*,dword);

        typename _Base::_StringBuffer _buf;
    };

    template<typename CharType>
    class shallow_string : public rtype_string<CharType>
    {
        typedef rtype_string<CharType> _Base;
    public:
        shallow_string();
        shallow_string(const CharType*);
        shallow_string(const _Base&);
        shallow_string(const shallow_string&);
        explicit shallow_string(dword allocSize);
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
    private:
        struct _StringBufferEx : _Base::_StringBuffer
        {
            _StringBufferEx();

            int reference;
        };

        virtual void _allocate(dword desiredSize);
        virtual void _deallocate();
        virtual void _copy(const CharType*,dword);        
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

    // typedefs for common string types
    typedef deep_string<char> str;
    typedef deep_string<wchar_t> wstr;

    // typedefs for generic common string types
    typedef rtype_string<char> generic_str;
    typedef rtype_string<wchar_t> generic_wstr;
}

// include out-of-line implementation
#include "rstring.tcc"

#endif
