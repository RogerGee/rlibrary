// rdynarray.h - provides dynamic array containers for rlibrary
#ifndef RDYNAMICARRAY_H
#define RDYNAMICARRAY_H
#include "rallocator.h" // gets rtypestypes.h
#include "rerror.h"

namespace rtypes
{
    template<typename T>
    class dynamic_array : protected rallocatorEx<T>
    {
    public:
        dynamic_array();
        explicit dynamic_array(dword iniSize);
        dynamic_array(dword iniSize,const T& defaultValue);

        T& operator [](dword index);
        const T& operator [](dword index) const;
        T& at(dword index);
        const T& at(dword index) const;

        T& front();
        const T& front() const;
        T& back();
        const T& back() const;

        void push_back(const T& element);
        T& pop_back();

        T& operator ++();
        T& operator ++(int);

        void resize(dword allocSize,bool exact = false);
        void clear();
        void reset();

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

// include out-of-line implementation
#include "rdynarray.tcc"

#endif
