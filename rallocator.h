// rallocator.h - provides allocators for rlibrary abstract data structures - indirect
#ifndef RALLOCATOR_H
#define RALLOCATOR_H
#include "rtypestypes.h"

namespace rtypes
{
    template<class T>
    class rallocator
    {
    protected:
        rallocator& operator =(const rallocator& obj)
        {
            if (&obj==this)
                return *this;
            _alloc(obj._allocSize);
            // copy as is
            for (size_type i = 0;i<obj._allocSize;i++)
                _data[i] = obj._data[i];
            return *this;
        }
        rallocator()
        {
            _data = 0;
            _allocSize = 0;
            _alloc();
        }
        explicit rallocator(size_type AllocSize)
        {
            _data = 0;
            _alloc(AllocSize);
        }
        rallocator(const rallocator& obj)
        {
            _data = 0;
            _allocSize = 0;
            _alloc(obj._allocSize);
            // copy as is
            for (size_type i = 0;i<obj._allocSize;i++)
                _data[i] = obj._data[i];
        }
        ~rallocator()
        {
            _dealloc();
        }
        T* _getData() const { return _data; }
        size_type _allocationSize() const { return _allocSize; }
        T* _alloc() // return ptr to new data
        {
            size_type newSize = (_allocSize==0 ? 4 : _allocSize*2);
            T* newData = new T[newSize];
            _copy(newData,_data);
            _dealloc();
            _data = newData;
            _allocSize = newSize;
            return newData;
        }
        T* _alloc(size_type desiredSize) // return ptr to new data
        {
            T* newData = new T[desiredSize];
            _copy(newData,_data);
            _dealloc();
            _data = newData;
            _allocSize = desiredSize;
            return newData;
        }
        virtual void _copy(T* copyTo,const T* copyFrom)
        {// default copying
            for (size_type i = 0;i<_allocSize;i++)
                copyTo[i] = copyFrom[i];
        }
        void _dealloc()
        {
            delete[] _data;
            _data = 0;
            _allocSize = 0;
        }
    private:
        T* _data;
        size_type _allocSize;
    };

    template<class T>
    class rallocatorEx // makes a virtual distinction between used and extra data
    {
    public:
        rallocatorEx& operator =(const rallocatorEx& obj)
        {
            if (&obj==this)
                return *this;
            delete[] _data;
            _sz = obj._sz;
            _extr = obj._extr;
            _data = new T[_allocationSize()];
            // copy as is
            for (size_type i = 0;i<_sz;i++)
                _data[i] = obj._data[i];
            return *this;
        }
    protected:
        rallocatorEx()
        {
            _data = 0;
            _sz = 0;
            _extr = 0;
            _virtAlloc(1); // _sz 1, _extr 3
            _virtAlloc(0); // _sz 0, _extr 4
        }
        explicit rallocatorEx(size_type AllocSize)
        {
            _data = 0;
            _sz = 0;
            _extr = 0;
            _virtAlloc(AllocSize); // will allocate more, but provides good buffering
        }
        rallocatorEx(const rallocatorEx& obj)
        {
            _sz = obj._sz;
            _extr = obj._extr;
            _data = new T[_allocationSize()];
            // copy as is
            for (size_type i = 0;i<_sz;i++)
                _data[i] = obj._data[i];
        }
        ~rallocatorEx()
        {
            _dealloc();
        }
        T* _getData() const { return _data; }
        size_type _size() const { return _sz; }
        size_type _extra() const { return _extr; }
        size_type _allocationSize() const { return _sz+_extr; }
        void _exactAlloc(size_type desiredSize)
        {
            if (desiredSize>0)
            {
                T* newData = new T[desiredSize];
                _copy(newData,_data);
                delete[] _data;
                _data = newData;
                _sz = desiredSize;
                _extr = 0;
            }
            else
                _dealloc();
        }
        bool _virtAlloc(size_type desiredSize) // returns a boolean indicating if the (de)allocation was virtual; all deallocations are virtual
        {
            if (desiredSize>_sz)
            {
                // an allocation is needed
                if (_allocationSize()>=desiredSize)
                {
                    // virtual allocation
                    size_type dif = desiredSize-_sz;
                    _extr -= dif;
                    _sz += dif;
                    return true;
                }
                else
                {
                    // reallocate to twice old allocation
                    size_type allocSize = _allocationSize();
                    size_type newSize = (allocSize==0 ? 4 : allocSize*2);
                    T* newData = new T[newSize];
                    _copy(newData,_data); // perform default or custom copy action
                    delete[] _data;
                    _data = newData;
                    _extr = newSize-_sz;
                    _virtAlloc(desiredSize);
                    return false;
                }
            }
            else if (desiredSize<_sz)
            {
                // virtual deallocation
                size_type dif = _sz-desiredSize;
                _extr += dif;
                _sz -= dif;
                //return true below
            }
            // else equal, no action
            return true; 
        }
        virtual void _copy(T* copyTo,const T* copyFrom)
        {// default copying
            for (size_type i = 0;i<_sz;i++)
                copyTo[i] = copyFrom[i];
        }
        void _dealloc()
        {
            delete[] _data;
            _data = 0;
            _sz = 0;
            _extr = 0;
        }
    private:
        T* _data;
        size_type _sz, _extr;
    };
}

#endif
