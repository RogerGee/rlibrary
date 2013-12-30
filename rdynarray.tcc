// rdynarray.tcc - out-of-line implementation for rdynarray

template<typename T>
rtypes::dynarray<T>::dynarray()
{
}
template<typename T>
rtypes::dynarray<T>::dynarray(dword iniSize)
{
    _exactAlloc(iniSize);
}
template<typename T>
rtypes::dynarray<T>::dynarray(dword iniSize,const T& defaultValue)
{
    T* data;
    _exactAlloc(iniSize);
    data = _getData();
    for (dword i = 0;i<iniSize;i++)
        data[i] = defaultValue;
}
template<typename T>
T& rtypes::dynarray<T>::operator [](dword i)
{
    return _getData()[i];
}
template<typename T>
const T& rtypes::dynarray<T>::operator [](dword i) const
{
    return _getData()[i];
}
template<typename T>
T& rtypes::dynarray<T>::at(dword i)
{
    if (i < _size())
        return _getData()[i];
    throw out_of_bounds_error();
}
template<typename T>
const T& rtypes::dynarray<T>::at(dword i) const
{
    if (i < _size())
        return _getData()[i];
    throw out_of_bounds_error();
}
template<typename T>
T& rtypes::dynarray<T>::front()
{
    if (0 < _size())
        return _getData()[0];
    throw element_not_found_error();
}
template<typename T>
const T& rtypes::dynarray<T>::front() const
{
    if (0 < _size())
        return _getData()[0];
    throw element_not_found_error();
}
template<typename T>
T& rtypes::dynarray<T>::back()
{
    dword sz = _size();
    if (0 <= --sz)
        return _getData()[sz];
    throw element_not_found_error();
}
template<typename T>
const T& rtypes::dynarray<T>::back() const
{
    dword sz = _size();
    if (0 <= --sz)
        return _getData()[sz];
    throw element_not_found_error();
}
template<typename T>
void rtypes::dynarray<T>::push_back(const T& elem)
{
    dword sz = _size();
    _virtAlloc(sz+1);
    _getData()[sz] = elem;
}
template<typename T>
T& rtypes::dynarray<T>::pop_back()
{
    dword sz = _size();
    _virtAlloc(--sz);
    return _getData()[sz];
}
template<typename T>
T& rtypes::dynarray<T>::operator ++()
{
    // add new default element and return reference
    dword sz = _size();
    _virtAlloc(sz+1);
    return _getData()[sz];
}
template<typename T>
T& rtypes::dynarray<T>::operator ++(int)
{
    // same as prefix
    dword sz = _size();
    _virtAlloc(sz+1);
    return _getData()[sz];
}
template<typename T>
void rtypes::dynarray<T>::resize(dword allocSize,bool exact)
{
    if (exact)
        _exactAlloc(allocSize);
    else
        _virtAlloc(allocSize);
}
template<typename T>
void rtypes::dynarray<T>::clear()
{
    _virtAlloc(0);
}
template<typename T>
void rtypes::dynarray<T>::reset()
{
    _dealloc();
}
