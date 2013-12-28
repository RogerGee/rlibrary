// rresource.tcc - template implementation for rresource.h

template<size_t BYTE_CNT>
rtypes::resource<BYTE_CNT>::resource()
{
    for (size_t i = 0;i<BYTE_CNT;i++)
        _idData[i] = 0;
}

template<size_t BYTE_CNT>
rtypes::resource<BYTE_CNT>::resource(rtypes::byte fillValue)
{
    for (size_t i = 0;i<BYTE_CNT;i++)
        _idData[i] = fillValue;
}

template<size_t BYTE_CNT>
rtypes::resource<BYTE_CNT>::resource(void* defaultValue)
{
    if (BYTE_CNT >= sizeof(defaultValue))
        reinterpret_cast<void*&>(_idData) = defaultValue;
    else
        throw bad_resource_assignment_error();
}

template<size_t BYTE_CNT>
template<typename T>
rtypes::resource<BYTE_CNT>& rtypes::resource<BYTE_CNT>::assign(const T& item)
{
    if (BYTE_CNT >= sizeof(T))
    {
        reinterpret_cast<T&>(*_idData) = item;
        return *this;
    }
    throw bad_resource_assignment_error();
}

template<size_t BYTE_CNT>
bool rtypes::operator ==(const rtypes::resource<BYTE_CNT>& left,const rtypes::resource<BYTE_CNT>& right)
{
    for (size_t i = 0;i<BYTE_CNT;i++)
        if (left._idData[i] != right._idData[i])
            return false;
    return true;
}

template<size_t BYTE_CNT>
bool rtypes::operator !=(const rtypes::resource<BYTE_CNT>& left,const rtypes::resource<BYTE_CNT>& right)
{
    for (size_t i = 0;i<BYTE_CNT;i++)
        if (left._idData[i] != right._idData[i])
            return true;
    return false;
}
