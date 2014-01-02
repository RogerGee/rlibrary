// rstring.tcc - out-of-line implementation for rstring

// rtypes::rtype_string<>
template<typename CharType>
rtypes::rtype_string<CharType>::rtype_string()
{
    _buffer = NULL;
}
template<typename CharType>
rtypes::rtype_string<CharType>::~rtype_string()
{
    // do nothing
}
template<typename CharType>
rtypes::rtype_string<CharType>& rtypes::rtype_string<CharType>::operator =(CharType c)
{
    _allocate(2);
    _buffer->data[0] = c;
    _nullTerm();
    return *this;
}
template<typename CharType>
rtypes::rtype_string<CharType>& rtypes::rtype_string<CharType>::operator =(const CharType* pcstr)
{
    _copy(pcstr);
    return *this;
}
template<typename CharType>
rtypes::rtype_string<CharType>& rtypes::rtype_string<CharType>::operator =(const rtype_string<CharType>& obj)
{
    if (this != &obj)
        _copy(obj._buffer->data,obj._buffer->size);
    return *this;
}
template<typename CharType>
rtypes::rtype_string<CharType>& rtypes::rtype_string<CharType>::operator +=(CharType c)
{
    push_back(c);
    return *this;
}
template<typename CharType>
rtypes::rtype_string<CharType>& rtypes::rtype_string<CharType>::operator +=(const CharType* pcstr)
{
    dword len = 0, lastSize = size();
    while (pcstr[len])
        ++len;
    _allocate(lastSize + ++len); // add 1 to len to account for the null-terminator
    for (dword i = 0;i<len;i++) // the terminator will be copied in from pcstr
        _buffer->data[i+lastSize] = pcstr[i];
    _nullTerm();
    return *this;
}
template<typename CharType>
rtypes::rtype_string<CharType>& rtypes::rtype_string<CharType>::operator +=(const rtype_string<CharType>& obj)
{
    dword lastSz = size();
    _allocate(lastSz+obj._buffer->size);
    for (dword i = 0;i<obj._buffer->size;i++)
        _buffer->data[i+lastSz] = obj[i];
    _nullTerm();
    return *this;
}
template<typename CharType>
void rtypes::rtype_string<CharType>::append(CharType t)
{
    // (a push_back synonym)
    _allocate(_buffer->size+1);
    _buffer->data[_buffer->size-2] = t;
    _nullTerm();
}
template<typename CharType>
void rtypes::rtype_string<CharType>::append(const CharType* pcstr)
{
    dword len = 0, lastSize = size();
    while (pcstr[len])
        ++len;
    _allocate(_buffer->size+len);
    for (dword i = 0;i<len;i++)
        _buffer->data[i+lastSize] = pcstr[i];
    _nullTerm();
}
template<typename CharType>
void rtypes::rtype_string<CharType>::append(const rtype_string& obj)
{
    dword lastSz = _buffer->size;
    _allocate(_buffer->size+obj._buffer->size);
    for (dword i = 0;i<obj._buffer->size;i++)
        _buffer->data[i+lastSz] = obj[i];
    _nullTerm();
}
template<typename CharType>
void rtypes::rtype_string<CharType>::push_back(CharType t)
{
    _allocate(_buffer->size+1);
    _buffer->data[_buffer->size-2] = t;
    _nullTerm();
}
template<typename CharType>
void rtypes::rtype_string<CharType>::clear()
{
    _allocate(1);
    _nullTerm();
}
template<typename CharType>
void rtypes::rtype_string<CharType>::reset()
{
    // this will reduce the allocation size to the
    // default allocation size constant
    _deallocate();
    _allocate(1); // still make this a null string
    _nullTerm();
}
template<typename CharType>
bool rtypes::rtype_string<CharType>::truncate(dword desiredSize)
{
    if (desiredSize >= _buffer->size)
        return false;
    _allocate(desiredSize+1);
    _nullTerm();
    return true;
}
template<typename CharType>
void rtypes::rtype_string<CharType>::resize(dword newSize)
{
    if (newSize != _buffer->size)
    {
        _allocate(newSize+1);
        _nullTerm();
    }
}
template<typename CharType>
void rtypes::rtype_string<CharType>::_copy(const CharType* pcstr)
{
    // calculate size, then invoke
    // virtual copy operation
    dword len = 0;
    while (pcstr[len] != 0)
        ++len;
    _copy(pcstr,++len); // include null-terminator
}
template<typename CharType>
void rtypes::rtype_string<CharType>::_copy(const CharType* pcstr,dword len)
{
    _allocate(len);
    for (dword i = 0;i<len;i++)
        _buffer->data[i] = pcstr[i];
}
template<typename CharType>
void rtypes::rtype_string<CharType>::_nullTerm()
{
    _buffer->data[_buffer->size-1] = 0;
}

// rtypes::rtype_string<>::_StringBuffer
template<typename CharType>
rtypes::rtype_string<CharType>::_StringBuffer::_StringBuffer()
{
    data = NULL;
    size = 0;
    extra = 0;
}
template<typename CharType>
rtypes::rtype_string<CharType>::_StringBuffer::~_StringBuffer()
{
    if (data != NULL)
        delete[] data;
}
template<typename CharType>
void rtypes::rtype_string<CharType>::_StringBuffer::allocate(dword desiredSize)
{
    if (desiredSize > size)
    {
        // an allocation is needed
        if (size+extra >= desiredSize)
        {
            // virtual allocation - enough space is already
            // allocated; adjust
            dword dif = desiredSize - size;
            extra -= dif;
            size += dif;
        }
        else
        {
            // reallocate to twice old allocation
            dword allocSize = size + extra;
            dword newSize = (allocSize==0 ? RSTRING_DEFAULT_ALLOCATION : allocSize*2);
            // adjust new size if not large enough
            if (newSize < desiredSize)
                newSize = desiredSize;
            // create new buffer
            CharType* newData = new CharType[newSize];
            if (data != NULL)
            {
                // copy old data
                for (dword i = 0;i<size;i++)
                    newData[i] = data[i];
                // delete old buffer
                delete[] data;
            }
            // assign new; recalculate extra space amount
            data = newData;
            extra = newSize - size;
            // adjust size
            allocSize = desiredSize - size;
            extra -= allocSize;
            size += allocSize;
        }
    }
    else if (desiredSize < size)
    {
        // virtual deallocation
        dword dif = size - desiredSize;
        extra += dif;
        size -= dif;
    }
    // else equal, no action
}
template<typename CharType>
void rtypes::rtype_string<CharType>::_StringBuffer::deallocate()
{
    if (data != NULL)
    {
        delete[] data;
        data = NULL;
        size = 0;
        extra = 0;
    }
}

// rtypes::deep_string<>
template<typename CharType>
rtypes::deep_string<CharType>::deep_string()
{
    // use statically-allocated buffer
    _buffer = &_buf;
    // allocate null string
    _buf.allocate(1);
    _nullTerm();
}
template<typename CharType>
rtypes::deep_string<CharType>::deep_string(const CharType* pcstr)
{
    // use statically-allocated buffer
    _buffer = &_buf;
    // copy string (along with null terminator)
    _copy(pcstr);
}
template<typename CharType>
rtypes::deep_string<CharType>::deep_string(const _Base& obj)
{
    // use statically-allocated buffer
    _buffer = &_buf;
    // copy string (along with null terminator)
    const typename _Base::_StringBuffer* pbuf = _getBuffer(obj);
    _copy(pbuf->data,pbuf->size);
}
template<typename CharType>
rtypes::deep_string<CharType>::deep_string(const deep_string& obj)
{
    // use statically-allocated buffer
    _buffer = &_buf;
    // copy string (along with null terminator)
    _copy(obj._buf.data,obj._buf.size);
}
template<typename CharType>
rtypes::deep_string<CharType>::deep_string(dword allocSize)
{
    // use statically-allocated buffer
    _buffer = &_buf;
    // get as close to the specified allocation size as possible,
    // accounting for the null terminator automatically
    _buf.data = new CharType[++allocSize];
    _buf.size = allocSize;
    _buf.extra = 0;
    _nullTerm();
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator =(CharType c)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator =(c);
    return *this;
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator =(const CharType* pcstr)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator =(pcstr);
    return *this;
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator =(const _Base& obj)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator =(obj);
    return *this;
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator =(const deep_string& obj)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator =(obj);
    return *this;
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator +=(CharType c)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(c);
    return *this;
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator +=(const CharType* pcstr)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(pcstr);
    return *this;
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator +=(const _Base& obj)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(obj);
    return *this;
}
template<typename CharType>
rtypes::deep_string<CharType>& rtypes::deep_string<CharType>::operator +=(const deep_string& obj)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(obj);
    return *this;
}
template<typename CharType>
void rtypes::deep_string<CharType>::_allocate(dword desiredSize)
{
    _buf.allocate(desiredSize);
}
template<typename CharType>
void rtypes::deep_string<CharType>::_deallocate()
{
    _buf.deallocate();
}

// rtypes::shallow_string<>
template<typename CharType>
rtypes::shallow_string<CharType>::shallow_string()
{
    // set up string buffer
    _buffer = new _StringBufferEx;
    // allocate default buffer for null string
    _buffer->allocate(1);
    _nullTerm();
}
template<typename CharType>
rtypes::shallow_string<CharType>::shallow_string(const CharType* pcstr)
{
    // set up new string buffer
    _buffer = new _StringBufferEx;
    // copy string (including null terminator) into buffer
    _copy(pcstr);
}
template<typename CharType>
rtypes::shallow_string<CharType>::shallow_string(const _Base& obj)
{
    // set up new string buffer
    _buffer = new _StringBufferEx;
    // copy string (including null terminator) into buffer
    const typename _Base::_StringBuffer* pbuf = _getBuffer(obj);
    _copy(pbuf->data,pbuf->size);
}
template<typename CharType>
rtypes::shallow_string<CharType>::shallow_string(const shallow_string& obj)
{
    // copy a reference to the buffer (shallow copy)
    _buffer = obj._buffer;
    ++static_cast<_StringBufferEx*>(_buffer)->reference; // increment reference
}
template<typename CharType>
rtypes::shallow_string<CharType>::shallow_string(dword allocSize)
{
    // set up new string buffer
    _buffer = new _StringBufferEx;
    // get as close to the specified allocation size as possible,
    // accounting for the null terminator automatically
    _buffer->data = new CharType[++allocSize];
    _buffer->size = allocSize;
    _buffer->extra = 0;
    _nullTerm();
}
template<typename CharType>
rtypes::shallow_string<CharType>::~shallow_string()
{
    // delete the buffer if the reference
    //  is about to be lost
    if (--static_cast<_StringBufferEx*>(_buffer)->reference == 0)
        delete _buffer;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator =(CharType t)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator =(t);
    return *this;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator =(const CharType* pcstr)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator =(pcstr);
    return *this;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator =(const _Base& obj)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator =(obj);
    return *this;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator =(const shallow_string& obj)
{
    // forget old reference and delete if last reference
    if (--static_cast<_StringBufferEx*>(_buffer)->reference == 0)
        delete _buffer;
    // copy reference and increment reference count
    _buffer = obj._buffer;
    ++static_cast<_StringBufferEx*>(_buffer)->reference;
    return *this;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator +=(CharType t)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(t);
    return *this;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator +=(const CharType* pcstr)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(pcstr);
    return *this;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator +=(const _Base& obj)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(obj);
    return *this;
}
template<typename CharType>
rtypes::shallow_string<CharType>& rtypes::shallow_string<CharType>::operator +=(const shallow_string& obj)
{
    // just invoke the base-class version
    static_cast<_Base*>(this)->operator +=(obj);
    return *this;
}
template<typename CharType>
void rtypes::shallow_string<CharType>::_allocate(dword desiredSize)
{
    _StringBufferEx* pbuf = static_cast<_StringBufferEx*>(_buffer);
    if (pbuf->reference > 1)
    {
        // buffer is about to be modified; lose the old one
        // and allocate a new buffer for the pending operation
        --pbuf->reference;
        _buffer = new _StringBufferEx;
        _buffer->allocate(desiredSize);
        // copy old data (as much or as little as possible)
        for (dword i = 0;i<desiredSize && i<pbuf->size;i++)
            _buffer->data[i] = pbuf->data[i];
    }
    else
    {
        // the reference is in sole use by this object,
        // therefore the buffer may be modified directly
        _buffer->allocate(desiredSize);
    }
}
template<typename CharType>
void rtypes::shallow_string<CharType>::_deallocate()
{
    _StringBufferEx* pbuf = static_cast<_StringBufferEx*>(_buffer);
    if (pbuf->reference > 1)
    {
        // buffer is about to be modified; lose the old one
        // and allocate a new buffer for the pending operation
        --pbuf->reference;
        _buffer = new _StringBufferEx; // leave in null state
    }
    else
    {
        // the reference is in sole use by this object,
        // therefore the buffer may be modified directly
        _buffer->deallocate();
    }
}
template<typename CharType>
CharType& rtypes::shallow_string<CharType>::_access(dword index)
{
    _StringBufferEx* pbuf = static_cast<_StringBufferEx*>(_buffer);
    if (pbuf->reference > 1)
    {
        // buffer is (potentially) about to be modified; lose the reference
        // and create a new buffer for the pending operation
        --pbuf->reference;
        _buffer = new _StringBufferEx;
        _buffer->allocate(pbuf->size);
        for (dword i = 0;i<pbuf->size;i++)
            _buffer->data[i] = pbuf->data[i];
    }
    return _buffer->data[index];
}

// rtypes::shallow_string<>::_StringBufferEx
template<typename CharType>
rtypes::shallow_string<CharType>::_StringBufferEx::_StringBufferEx()
{
    reference = 1;
}

// comparison operator overloads for rtype_string
template<typename CharType>
bool rtypes::operator ==(const rtype_string<CharType>& left,const rtype_string<CharType>& right)
{
    if (left.size() == right.size())
    {
        for (dword i = 0;i<left.size();i++)
            if (left[i] != right[i])
                return false;
        return true;
    }
    return false;
}
template<typename CharType>
bool rtypes::operator ==(const rtype_string<CharType>& left,const CharType* right)
{
    dword len = 0;
    while (right[len])
        ++len;
    if (left.size() == len)
    {
        for (dword i = 0;i<left.size();i++)
            if (left[i] != right[i])
                return false;
        return true;
    }
    return false;
}
template<typename CharType>
bool rtypes::operator ==(const CharType* left,const rtype_string<CharType>& right)
{
    dword len = 0;
    while (left[len])
        len++;
    if (len == right.size())
    {
        for (dword i = 0;i<len;i++)
            if (left[i] != right[i])
                return false;
        return true;
    }
    return false;

}
template<typename CharType>
bool rtypes::operator !=(const rtype_string<CharType>& left,const rtype_string<CharType>& right)
{
    return !operator ==(left,right);
}
template<typename CharType>
bool rtypes::operator !=(const rtype_string<CharType>& left,const CharType* right)
{
    return !operator ==(left,right);
}
template<typename CharType>
bool rtypes::operator !=(const CharType* left,const rtype_string<CharType>& right)
{
    return !operator ==(left,right);
}
template<typename CharType>
bool rtypes::operator <(const rtype_string<CharType>& left,const rtype_string<CharType>& right)
{
    for (dword i = 0;i<left.size() && i<right.size();i++)
        if ( left[i] < right[i] )
            return true;
        else if ( left[i] > right[i] )
            return false;
    return left.size() < right.size();
}
template<typename CharType>
bool rtypes::operator <(const rtype_string<CharType>& left,const CharType* right)
{
    dword len = 0;
    while (right[len])
        len++;
    for (dword i = 0;i<left.size() && i<len;i++)
        if ( left[i] < right[i] )
            return true;
        else if ( left[i] > right[i] )
            return false;
    return left.size() < len;
}
template<typename CharType>
bool rtypes::operator <(const CharType* left,const rtype_string<CharType>& right)
{
    dword len = 0;
    while (left[len])
        len++;
    for (dword i = 0;i<len && i<right.size();i++)
        if ( left[i] < right[i] )
            return true;
        else if ( left[i] > right[i] )
            return false;
    return len < right.size();
}
template<typename CharType>
bool rtypes::operator >=(const rtype_string<CharType>& left,const rtype_string<CharType>& right)
{
    return !operator <(left,right);
}
template<typename CharType>
bool rtypes::operator >=(const rtype_string<CharType>& left,const CharType* right)
{
    return !operator <(left,right);
}
template<typename CharType>
bool rtypes::operator >=(const CharType* left,const rtype_string<CharType>& right)
{
    return !operator <(left,right);
}
template<typename CharType>
bool rtypes::operator >(const rtype_string<CharType>& left,const rtype_string<CharType>& right)
{
    for (dword i = 0;i<left.size() && i<right.size();i++)
        if ( left[i] > right[i] )
            return true;
        else if ( left[i] < right[i] )
            return false;
    return left.size() > right.size();
}
template<typename CharType>
bool rtypes::operator >(const rtype_string<CharType>& left,const CharType* right)
{
    dword len = 0;
    while (right[len])
        len++;
    for (dword i = 0;i<len && i<left.size();i++)
        if ( left[i] > right[i] )
            return true;
        else if ( left[i] < right[i] )
            return false;
    return left.size() > len;
}
template<typename CharType>
bool rtypes::operator >(const CharType* left,const rtype_string<CharType>& right)
{
    dword len = 0;
    while (left[len])
        len++;
    for (dword i = 0;i<len && i<right.size();i++)
        if ( left[i] > right[i] )
            return true;
        else if ( left[i] < right[i] )
            return false;
    return len>right.size();
}
template<typename CharType>
bool rtypes::operator <=(const rtype_string<CharType>& left,const rtype_string<CharType>& right)
{
    return !operator >(left,right);
}
template<typename CharType>
bool rtypes::operator <=(const rtype_string<CharType>& left,const CharType* right)
{
    return !operator >(left,right);
}
template<typename CharType>
bool rtypes::operator <=(const CharType* left,const rtype_string<CharType>& right)
{
    return !operator >(left,right);
}

// concatenation operator overloads for rtype_string
// return types match concrete types derived from rtype_string
template<typename CharType>
rtypes::deep_string<CharType> rtypes::operator +(const rtype_string<CharType>& left,const rtype_string<CharType>& right)
{
    deep_string<CharType> result(left);
    result += right;
    return result;
}
template<typename CharType>
rtypes::deep_string<CharType> rtypes::operator +(const rtype_string<CharType>& left,const CharType* right)
{
    deep_string<CharType> result(left);
    result += right;
    return result;
}
template<typename CharType>
rtypes::deep_string<CharType> rtypes::operator +(const CharType* left,const rtype_string<CharType>& right)
{
    deep_string<CharType> result(left);
    result += right;
    return result;
}
