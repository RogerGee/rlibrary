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
        explicit dynamic_array(size_type iniSize);
        dynamic_array(size_type iniSize,const T& defaultValue);

        T& operator [](size_type index);
        const T& operator [](size_type index) const;
        T& at(size_type index);
        const T& at(size_type index) const;

        T& front();
        const T& front() const;
        T& back();
        const T& back() const;

        void push_back(const T& element);
        T& pop_back();

        T& operator ++();
        T& operator ++(int);

        void resize(size_type allocSize,bool exact = false);
        void clear();
        void reset();

        bool is_empty() const
        { return _size()==0; }
        size_type size() const
        { return _size(); }
        size_type capacity() const
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
