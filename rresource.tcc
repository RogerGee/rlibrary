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
    for (size_t i = 0;i<BYTE_CNT;i++)
    {
        if (i < sizeof(void*))
            _idData[i] = (reinterpret_cast<size_t>(defaultValue) >> 8*i) & 0xff;
        else
            _idData[i] = 0;
    }
}

template<size_t BYTE_CNT>
template<typename T>
rtypes::resource<BYTE_CNT>& rtypes::resource<BYTE_CNT>::assign(const T& item)
{
    for (size_t i = 0;i<BYTE_CNT;i++)
    {
        if (i < sizeof(T))
            _idData[i] = (size_t(item) >> 8*i) & 0xff;
        else
            _idData[i] = 0;
    }       
    return *this;
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
