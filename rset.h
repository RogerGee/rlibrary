// rset.h - provides set containers for rlibrary
#ifndef RSET_H
#define RSET_H
#include "rlist.h"

namespace rtypes
{
    template<typename T>
    class set
    {
    public:
        /* insert( element )
         *  adds the specified element to the set
         *  if it does not exist already; returns false
         *  if it already exists
         */
        bool insert(const T& elem)
        {
            if ( !contains(elem) )
            {
                _l.push_back(elem);
                return true;
            }
            return false;
        }
                
        /* remove( element )
         *  removes the specified element if it exists;
         */
        void remove(const T& elem)
        { _l.remove(elem); }
                
        /* empty( )
         *  removes all elements from the set
         */
        void empty()
        { _l.clear(); }
                
        /* contains( element )
         *  determines if the element is in the set
         */
        bool contains(const T& elem) const
        { return _l.find(elem)!=_l.end(); }
                
        /* is_empty( )
         *  determines if there are any elements in the set
         */
        bool is_empty() const
        { return _l.length()==0; }
                
        /* size( )
         *  returns the number of elements in the set
         */
        size_type size() const
        { return _l.size(); }
                
        bool operator ==(const set& obj) const
        {
            typename list<T>::iterator e = _l.end();
            for (typename list<T>::iterator i = _l.begin();i!=e;i++)
                if ( !obj.contains(*i) )
                    return false;
            return true;
        }
        bool operator !=(const set& obj) const
        { return !(*this==obj); }
    private:
        list<T> _l;
    };
}

#endif
