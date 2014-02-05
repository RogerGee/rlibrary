// rlist.h - version 2
#ifndef RLIST_H
#define RLIST_H
#include "rerror.h"
#include "rtypestypes.h"
#include "rnode.h"

namespace rtypes
{
    template<typename T>
    class _list_iterator
    {
        typedef _rnode_double<T>* _NodePtr;
        typedef _list_iterator _Self;
        typedef T& _Ref;
        typedef T* _Ptr;
    public:
        _list_iterator()
            : _n(NULL) {}
        explicit _list_iterator(_NodePtr n)
            : _n(n) {}

        _Self& operator ++()
        {
            _n = _n->next;
            return *this;
        }
        _Self operator ++(int)
        {
            _Self tmp = *this;
            _n = _n->next;
            return tmp;
        }

        _Self& operator --()
        {
            _n = _n->prev;
            return *this;
        }
        _Self operator --(int)
        {
            _Self tmp = *this;
            _n = _n->prev;
            return tmp;
        }

        _Ref operator *() const
        { return _n->item; }
        _Ptr operator ->() const
        { return &_n->item; }

        bool operator ==(const _Self& obj) const
        { return _n==obj._n; }
        bool operator !=(const _Self& obj) const
        { return _n!=obj._n; }

        _NodePtr _node() const
        { return _n; }
    private:
        _NodePtr _n;
    };

    template<typename T>
    class _list_const_iterator
    {
        typedef const _rnode_double<T>* _NodePtr;
        typedef _list_const_iterator _Self;
        typedef const T& _Ref;
        typedef const T* _Ptr;
    public:
        _list_const_iterator()
            : _n(NULL) {}
        explicit _list_const_iterator(_NodePtr n)
            : _n(n) {}

        _Self& operator ++()
        {
            _n = _n->next;
            return *this;
        }
        _Self operator ++(int)
        {
            _Self tmp = *this;
            _n = _n->next;
            return tmp;
        }

        _Self& operator --()
        {
            _n = _n->prev;
            return *this;
        }
        _Self operator --(int)
        {
            _Self tmp = *this;
            _n = _n->prev;
            return tmp;
        }

        _Ref operator *() const
        { return _n->item; }
        _Ptr operator ->() const
        { return &_n->item; }

        bool operator ==(const _Self& obj) const
        { return _n==obj._n; }
        bool operator !=(const _Self& obj) const
        { return _n!=obj._n; }

        _NodePtr _node() const
        { return _n; }
    private:
        _NodePtr _n;
    };

    template<typename T>
    bool operator ==(const _list_iterator<T>& left,
        const _list_const_iterator<T>& right)
    { return left._node()==right._node(); }
    template<typename T>
    bool operator !=(const _list_iterator<T>& left,
        const _list_const_iterator<T>& right)
    { return left._node()!=right._node(); }

    template<typename T>
    class list
    {
        typedef _rnode_double<T> _Node;
        typedef _rnode_double_link<_Node> _Dummy; // yes, I'll shoot myself in the foot here
        typedef list _Self;
    public:
        typedef rtypes::_list_iterator<T> iterator;
        typedef rtypes::_list_const_iterator<T> const_iterator;

        list()
        {
            _root << _root;
            _root >> _root;
            _sz = 0;
        }
        list(const _Self& obj)
        {
            _root << _root;
            _root >> _root;
            _sz = 0;
            _copy(obj);
        }
        ~list()
        { _deleteElements(); }

        _Self& operator =(const _Self& obj)
        {
            if (this != &obj)
                _copy(obj);
            return *this;
        }

        /* ++ prefix
         *  grow front by one default element
         *  and return a reference to it
         */
        T& operator ++()
        {
            grow_front(1);
            return *begin();
        }

        /* ++ postfix
         *  grow back by one default element
         *  and return a reference to it
         */
        T& operator ++(int)
        {
            grow_back(1);
            return _root.prev->item;
        }

        /* swap( list& obj )
         *  exchange contents of lists
         */
        void swap(_Self& obj);

        /* grow_front( cnt )
         *  add n number of default elements to the front
         *  of the list
         */
        void grow_front(size_type n);

        /* grow_back( cnt )
         *  add n number of default elements to the back
         *  of the list
         */
        void grow_back(size_type n);

        /* push_front( value )
         *  insert element at beginning of list
         */
        void push_front(const T& value);

        /* push_back( value )
         *  insert element at end of list
         */
        void push_back(const T& value);

        /* remove( value )
         *  remove all elements of value
         */
        void remove(const T& value);

        /* remove_at( iterator )
         *  remove element pointed to by iterator
         */
        void remove_at(iterator);

        /* clear( )
         *  remove all elements from the list
         */
        void clear()
        { _deleteElements(); }

        /* sort( )
         *  sorts the elements from least to greatest
         */
        void sort();

        /* find( value )
         *  returns an iterator to the first occurance of
         *  'value' in the list OR end() if not found
         * find( value, start )
         *  returns an iterator to the next occurance of
         *  'value' in the list after 'start' or end() if
         *  not found
         */
        iterator find(const T& value);
        iterator find(const T& value,iterator start);
        const_iterator find(const T& value) const;
        const_iterator find(const T& value,const_iterator start) const;

        /* insert( position,value )
         *  inserts the specified element into the list
         *  at the specified location
         * insert( position,times,value )
         *  inserts a copy of value a specified number of
         *  times at the specified location
         */
        void insert(iterator position,const T& value);
        void insert(iterator position,size_type times,const T& value);

        T& insert_emplace(iterator position);

        /* front( )
         *  get reference to first element in list
         */
        T& front()
        { return _root.next->item; }
        const T& front() const
        { return _root.next->item; }

        /* back( )
         *  get reference to last element in list
         */
        T& back()
        { return _root.prev->item; }
        const T& back() const
        { return _root.prev->item; }

        /* pop_front( )
         *  return front element and remove it from list
         */
        T pop_front();

        /* pop_back()
         *  return back element and remove it from list
         */
        T pop_back();

        /* is_empty( )
         *  determine if list is empty
         */
        bool is_empty() const
        { return _root.next == &_root; }

        /* size( ) [alt: length( )]
         *  get length of list with O(1) complexity
         */
        size_type size() const
        { return _sz; }
        size_type length() const
        { return _sz; }

        /* begin( )
         *  gets iterator to start of list
         * end( )
         *  gets iterator one past end of list
         */
        iterator begin()
        { return iterator(_root.next); }
        const_iterator begin() const
        { return const_iterator(_root.next); }
        iterator end()
        { return iterator(static_cast<_Node*>(&_root)); }
        const_iterator end() const
        { return const_iterator(static_cast<const _Node*>(&_root)); }
    private:
        _Dummy _root;
        size_type _sz;

        void _deleteElements();
        void _copy(const _Self&);
        static void _qsortRec(_Node**,size_type);
    };

    // operator overloads for list<T>
    template<typename T>
    bool operator ==(const list<T>&,const list<T>&);
    template<typename T>
    bool operator !=(const list<T>&,const list<T>&);
}

#include "rlist.tcc"

#endif
