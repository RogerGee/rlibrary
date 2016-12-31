// rdynarray.tcc - out-of-line implementation for rdynarray

template<typename T>
rtypes::dynamic_array<T>::dynamic_array()
{
}
template<typename T>
rtypes::dynamic_array<T>::dynamic_array(size_type iniSize)
{
    _exactAlloc(iniSize);
}
template<typename T>
rtypes::dynamic_array<T>::dynamic_array(size_type iniSize,const T& defaultValue)
{
    T* data;
    _exactAlloc(iniSize);
    data = _getData();
    for (size_type i = 0;i<iniSize;i++)
        data[i] = defaultValue;
}
template<typename T>
T& rtypes::dynamic_array<T>::operator [](size_type i)
{
    return _getData()[i];
}
template<typename T>
const T& rtypes::dynamic_array<T>::operator [](size_type i) const
{
    return _getData()[i];
}
template<typename T>
T& rtypes::dynamic_array<T>::at(size_type i)
{
    if (i < _size())
        return _getData()[i];
    throw out_of_bounds_error();
}
template<typename T>
const T& rtypes::dynamic_array<T>::at(size_type i) const
{
    if (i < _size())
        return _getData()[i];
    throw out_of_bounds_error();
}
template<typename T>
T& rtypes::dynamic_array<T>::front()
{
    if (0 < _size())
        return _getData()[0];
    throw element_not_found_error();
}
template<typename T>
const T& rtypes::dynamic_array<T>::front() const
{
    if (0 < _size())
        return _getData()[0];
    throw element_not_found_error();
}
template<typename T>
T& rtypes::dynamic_array<T>::back()
{
    size_type sz = _size();
    if (0 == --sz)
        return _getData()[sz];
    throw element_not_found_error();
}
template<typename T>
const T& rtypes::dynamic_array<T>::back() const
{
    size_type sz = _size();
    if (0 == --sz)
        return _getData()[sz];
    throw element_not_found_error();
}
template<typename T>
void rtypes::dynamic_array<T>::push_back(const T& elem)
{
    size_type sz = _size();
    _virtAlloc(sz+1);
    _getData()[sz] = elem;
}
template<typename T>
T& rtypes::dynamic_array<T>::pop_back()
{
    size_type sz = _size();
    _virtAlloc(--sz);
    return _getData()[sz];
}
template<typename T>
T& rtypes::dynamic_array<T>::operator ++()
{
    // add new default element and return reference
    size_type sz = _size();
    _virtAlloc(sz+1);
    return _getData()[sz];
}
template<typename T>
T& rtypes::dynamic_array<T>::operator ++(int)
{
    // same as prefix
    size_type sz = _size();
    _virtAlloc(sz+1);
    return _getData()[sz];
}
template<typename T>
void rtypes::dynamic_array<T>::resize(size_type allocSize,bool exact)
{
    if (exact)
        _exactAlloc(allocSize);
    else
        _virtAlloc(allocSize);
}
template<typename T>
void rtypes::dynamic_array<T>::clear()
{
    _virtAlloc(0);
}
template<typename T>
void rtypes::dynamic_array<T>::reset()
{
    _dealloc();
}
