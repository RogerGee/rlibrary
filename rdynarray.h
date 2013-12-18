// rdynarray.h - provides dynamic array containers for rlibrary
#ifndef RDYNAMICARRAY_H
#define RDYNAMICARRAY_H
#include "rallocator.h" // gets rtypestypes.h
#include "rerror.h"

namespace rtypes
{
    template<class T>
    class dynarray : protected rallocatorEx<T>
    {
    public:
        dynarray() {}
        explicit dynarray(dword iniSize)
        { _exactAlloc(iniSize); }
        dynarray(dword iniSize,const T& k)
        {
            T* data;
            _exactAlloc(iniSize);
            data = _getData();
            for (dword i = 0;i<iniSize;i++)
                data[i] = k;
        }
                
        dynarray(const T& defaultValue,dword iniSize)
        {
            T* data = _getData();
            _exactAlloc(iniSize);
            for (dword i = 0;i<iniSize;i++)
                data[i] = defaultValue;
        }
                
        T& operator [](dword i)
        { return _getData()[i]; }
        const T& operator [](dword i) const
        { return _getData()[i]; }
        T& at(dword i)
        {
            if (i < _size())
                return _getData()[i];
            throw out_of_bounds_error();
        }
        const T& at(dword i) const
        {
            if (i < _size())
                return _getData()[i];
            throw out_of_bounds_error();
        }
        T& front()
        {
            if (0 < _size())
                return _getData()[0];
            throw element_not_found_error();
        }
        const T& front() const
        {
            if (0 < _size())
                return _getData()[0];
            throw element_not_found_error();
        }
        T& back()
        {
            dword sz = _size();
            if (0 <= --sz)
                return _getData()[sz];
            throw element_not_found_error();
        }
        const T& back() const
        {
            dword sz = _size();
            if (0 <= --sz)
                return _getData()[sz];
            throw element_not_found_error();
        }
                
        void push_back(const T& elem)
        {
            dword sz = _size();
            _virtAlloc(sz+1);
            _getData()[sz] = elem;
        }
                
        // add new default element and return reference
        T& operator ++()
        {
            dword sz = _size();
            _virtAlloc(sz+1);
            return _getData()[sz];
        }
        T& operator ++(int)
        {
            dword sz = _size();
            _virtAlloc(sz+1);
            return _getData()[sz-1]; // return previous back
        }
                
        T& pop_back()
        {
            dword sz = _size();
            _virtAlloc(--sz);
            return _getData()[sz];
        }
        const T& pop_back() const
        {
            dword sz = _size();
            _virtAlloc(--sz);
            return _getData()[sz];
        }
                
        void resize(dword allocSize,bool exact = false)
        {
            if (exact)
                _exactAlloc(allocSize);
            else
                _virtAlloc(allocSize);
        }
        void clear()
        { _virtAlloc(0); }
        void reset()
        { _dealloc(); }
                
        bool is_empty() const
        { return _size()==0; }
        dword size() const
        { return _size(); }
        dword capacity() const
        { return _allocationSize(); }
    protected:
        using rallocatorEx<T>::_exactAlloc;
        using rallocatorEx<T>::_virtAlloc;
        using rallocatorEx<T>::_allocationSize;
        using rallocatorEx<T>::_size;
        using rallocatorEx<T>::_getData;
        using rallocatorEx<T>::_dealloc;
    };
}

#endif
