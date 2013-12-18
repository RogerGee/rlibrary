// rnode.h - provides basic node types for container implementation - indirect
#ifndef RNODE_H
#define RNODE_H

#ifndef NULL
#define NULL 0
#endif

namespace rtypes
{
    template<typename T>
    struct _rnode_single_link
    {
        _rnode_single_link()
            : next(NULL) {}
                
        _rnode_single_link& operator >>(T& obj)
        {
            this->next = &obj;
            return *this;
        }
                
        _rnode_single_link& operator >>(_rnode_single_link& obj)
        {
            this->next = static_cast<T*>(&obj);
            return *this;
        }
                
        void reset()
        { next = NULL; }

        T *next;
    };

    template<typename T>
    struct _rnode_double_link
    {
        _rnode_double_link()
            : next(NULL), prev(NULL) {}

        _rnode_double_link& operator >>(T& obj)
        {
            this->next = &obj;
            return *this;
        }
        _rnode_double_link& operator <<(T& obj)
        {
            this->prev = &obj;
            return *this;
        }
                
        _rnode_double_link& operator >>(_rnode_double_link& obj)
        {
            this->next = static_cast<T*>(&obj);
            return *this;
        }
        _rnode_double_link& operator <<(_rnode_double_link& obj)
        {
            this->prev = static_cast<T*>(&obj);
            return *this;
        }
                
        void reset()
        {
            next = NULL;
            prev = NULL;
        }

        T *next, *prev;
    };

    template<typename T>
    struct _rnode_single : _rnode_single_link< _rnode_single<T> >
    {
        _rnode_single() {}
        _rnode_single(const T& value)
            : item(value) {}

        T item;
    };

    template<typename T>
    struct _rnode_double : _rnode_double_link< _rnode_double<T> >
    {
        _rnode_double() {}
        _rnode_double(const T& value)
            : item(value) {}

        T item;
    };

    /* Copy-Protected nodes
     *      copy-on-write if non-const
     *  copy-protected if const
     */
    template<typename T>
    struct _cp_rnode_base
    {
    protected:
        _cp_rnode_base()
        {
            pRef = new int(1);
            pItem = new T;
        }
        _cp_rnode_base(const T& value)
        {
            pRef = new int(1);
            pItem = new T(value);
        }
        _cp_rnode_base(const _cp_rnode_base& obj)
        {
            pRef = &( ++(*obj.pRef) );
            pItem = obj.pItem;
        }
        ~_cp_rnode_base()
        {
            if ( --(*pRef) <= 0 )
            {
                delete pItem;
                delete pRef;
            }
        }

        _cp_rnode_base& operator =(const _cp_rnode_base& obj)
        {
            if (this != &obj)
            {
                if ( --(*pRef) <= 0 )
                {
                    delete pItem;
                    delete pRef;
                }
                pRef = &( ++(*obj.pRef) );
                pItem = obj.pItem;
            }
            return *this;
        }

    public:
        T& operator *()
        {
            // copy-on-write (supposed write)
            if ( *pRef > 1 )
            {
                --(*pRef);
                pItem = new T(*pItem);
                pRef = new int(1);
            }
            return *pItem;
        }
        const T& operator *() const
        {
            return *pItem;
        }

    private:
        T* pItem;
        int* pRef;
    };

    template<typename T>
    struct _cp_rnode_single : _cp_rnode_base<T>, _rnode_single_link< _cp_rnode_single<T> >
    {
        _cp_rnode_single() {}
        _cp_rnode_single(const T& value)
            : _cp_rnode_base<T>(value) {}
    };

    template<typename T>
    struct _cp_rnode_double : _cp_rnode_base<T>, _rnode_double_link< _cp_rnode_double<T> >
    {
        _cp_rnode_double() {}
        _cp_rnode_double(const T& value)
            : _cp_rnode_base<T>(value) {}
    };
}

#endif
