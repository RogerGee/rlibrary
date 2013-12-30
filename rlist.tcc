// rlist.tcc - rlist out-of-line implementation

template<typename T>
void rtypes::list<T>::swap(_Self& obj)
{
    T tmp; // in case T has a lengthy constructor, I place it here so it runs once
    iterator thisIter = begin(),
        thatIter = obj.begin(),
        thisEnd = end(),
        thatEnd = obj.end();
    while (thisIter!=thisEnd || thatIter!=thatEnd)
    {
        if (thisIter != thisEnd)
        {
            tmp = *thisIter;
            if (thatIter != thatEnd)
            {
                *thisIter++ = *thatIter;
                *thatIter++ = tmp;
            }
            else
            {
                obj.push_back(tmp);
                this->remove_at(thisIter++);
            }
        }
        else if (thatIter != thatEnd)
        {
            tmp = *thatIter;
            obj.remove_at(thatIter++);
            this->push_back(tmp);
        }
    }
}

template<typename T>
void rtypes::list<T>::grow_front(dword cnt)
{
    for (dword i = 0;i<cnt;i++)
    {
        _Node *n = new _Node;
        *n >> *_root.next;
        *n << _root;
        *_root.next << *n;
        _root >> *n;
        ++_sz;
    }
}

template<typename T>
void rtypes::list<T>::grow_back(dword cnt)
{
    for (dword i = 0;i<cnt;i++)
    {
        _Node *n = new _Node;
        *n >> _root;
        *n << *_root.prev;
        *_root.prev >> *n;
        _root << *n;
        ++_sz;
    }
}

template<typename T>
void rtypes::list<T>::push_front(const T& value)
{
    _Node *n = new _Node(value);
    *n >> *_root.next;
    *n << _root;
    *_root.next << *n;
    _root >> *n;
    ++_sz;
}

template<typename T>
void rtypes::list<T>::push_back(const T& value)
{
    _Node *n = new _Node(value);
    *n >> _root;
    *n << *_root.prev;
    *_root.prev >> *n;
    _root << *n;
    ++_sz;
}

template<typename T>
void rtypes::list<T>::remove(const T& value)
{
    iterator iter = begin(), theEnd = end();
    while (iter != theEnd)
    {
        if (*iter == value)
            remove_at(iter++);
        else
            iter++;
    }
}

template<typename T>
void rtypes::list<T>::remove_at(iterator iter)
{
    _Node* n = iter._node();
    if (n == &_root)
        throw bad_iterator_error();
    *n->prev >> *n->next;
    *n->next << *n->prev;
    delete n;
    --_sz;
}

template<typename T>
void rtypes::list<T>::sort()
{
    _Node *beg = _root.next, *walker;
    // this recursive function expectes a non-circular
    // doubly-linked list; therefore the links must be broken
    _root.prev->next = NULL;
    beg->prev = NULL;
    _qsortRec(&beg,_sz);
    _root >> *beg;
    *beg << _root;
    walker = beg;
    while (walker->next != NULL)
        walker = walker->next;
    *walker >> _root;
    _root << *walker;
}

template<typename T>
typename rtypes::list<T>::iterator rtypes::list<T>::find(const T& value)
{
    iterator e = end();
    for (iterator i = begin();i!=e;i++)
        if (*i == value)
            return i;
    return e;
}

template<typename T>
typename rtypes::list<T>::iterator rtypes::list<T>::find(const T& value,iterator start)
{
    iterator e = end();
    for (;start!=e;start++)
        if (*start == value)
            return start;
    return e;
}

template<typename T>
typename rtypes::list<T>::const_iterator rtypes::list<T>::find(const T& value) const
{
    const_iterator e = end();
    for (const_iterator i = begin();i!=e;i++)
        if (*i == value)
            return i;
    return e;
}

template<typename T>
typename rtypes::list<T>::const_iterator rtypes::list<T>::find(const T& value,const_iterator start) const
{
    const_iterator e = end();
    for (;start!=e;start++)
        if (*start == value)
            return start;
    return e;
}

template<typename T>
void rtypes::list<T>::insert(iterator position,const T& value)
{
    _Node *nw = new _Node(value);
    _Node *n = position._node();
    *n->prev >> *nw;
    *nw << *n->prev;
    *nw >> *n;
    *n << *nw;
    ++_sz;
}

template<typename T>
void rtypes::list<T>::insert(iterator position,dword times,const T& value)
{
    for (dword i = 0;i<times;i++)
        insert(position++,value);
}

template<typename T>
T& rtypes::list<T>::insert_emplace(iterator position)
{
    _Node* nw = new _Node;
    _Node* n = position._node();
    *n->prev >> *nw;
    *nw << *n->prev;
    *nw >> *n;
    *n << *nw;
    ++_sz;
    return nw->item;
}

template<typename T>
T rtypes::list<T>::pop_front()
{
    T tmp = front();
    remove_at( begin() );
    return tmp;
}

template<typename T>
T rtypes::list<T>::pop_back()
{
    T tmp = back();
    remove_at( --end() );
    return tmp;
}

template<typename T>
void rtypes::list<T>::_deleteElements()
{
    if ( !is_empty() )
    {
        _Node* n = _root.next;
        while (n != &_root)
        {
            _Node* nxt = n->next;
            delete n;
            n = nxt;
        }
        _root << _root;
        _root >> _root;
        _sz = 0;
    }
}

template<typename T>
void rtypes::list<T>::_copy(const _Self& obj)
{
    iterator ownBeg = begin();
    const iterator ownEnd = end();
    const_iterator ibeg;
    const const_iterator iend = obj.end();
    ibeg = obj.begin();
    while (ibeg != iend)
    {
        if (ownBeg == ownEnd)
            push_back( *ibeg );
        else
            *ownBeg++ = *ibeg;
        ibeg++;
    }
}

template<typename T>
/* static */void rtypes::list<T>::_qsortRec(_Node** ppHead,dword sz)
{
    if (sz > 1)
    {
        _Node   *cpy,
            *piv = *ppHead,
            *eq = NULL,
            *less = NULL,
            *grtr = NULL,
            *eqPos = NULL,
            *lessPos = NULL,
            *grtrPos = NULL;
        dword   lessSz = 0,
            grtrSz = 0;
        for (dword i = 0;i<sz/2;i++)
            piv = piv->next;
        if (piv->next != NULL)
            piv->next->prev = piv->prev;
        if (piv->prev != NULL)
        {
            piv->prev->next = piv->next;
            piv->prev = NULL;
        }
        piv->next = NULL;
        cpy = *ppHead;
        eq = piv;
        eqPos = piv;
        while (cpy != NULL)
        {
            _Node *nxt = cpy->next;
            if (cpy->item < piv->item)
            {
                if (lessPos==NULL)
                {
                    less = cpy;
                    lessPos = cpy;
                    cpy->prev = NULL;
                    cpy->next = NULL;
                }
                else
                {
                    lessPos->next = cpy;
                    cpy->prev = lessPos;
                    cpy->next = NULL;
                    lessPos = cpy;
                }
                ++lessSz;
            }
            else if (cpy->item > piv->item)
            {
                if (grtrPos==NULL)
                {
                    grtr = cpy;
                    grtrPos = cpy;
                    cpy->prev = NULL;
                    cpy->next = NULL;
                }
                else
                {
                    grtrPos->next = cpy;
                    cpy->prev = grtrPos;
                    cpy->next = NULL;
                    grtrPos = cpy;
                }
                ++grtrSz;
            }
            else
            {
                eqPos->next = cpy;
                cpy->prev = eqPos;
                cpy->next = NULL;
                eqPos = cpy;
            }
            cpy = nxt;
        }
        _qsortRec(&less,lessSz);
        _qsortRec(&grtr,grtrSz);
        lessPos = less;
        grtrPos = grtr;
        if (lessPos != NULL)
            while (lessPos->next)
                lessPos = lessPos->next;
        if (grtrPos != NULL)
            while (grtrPos->next)
                grtrPos = grtrPos->next;
        if (less)
        {
            *ppHead = less;
            lessPos->next = eq;
        }
        else
            *ppHead = eq;
        eq->prev = lessPos;
        eqPos->next = grtr;
        if (grtr)
            grtr->prev = eqPos;
    }
}

/*
 * RList operator overloads
 */

template<typename T>
bool rtypes::operator ==(const rtypes::list<T>& left,const rtypes::list<T>& right)
{
    if (left.size() == right.size())
    {
        for (typename list<T>::const_iterator l = left.begin(), r = right.begin(), e = left.end();l!=e;l++,r++)
            if (*l != *r)
                return false;   
        return true;
    }
    return false;
}
template<typename T>
bool rtypes::operator !=(const rtypes::list<T>& left,const rtypes::list<T>& right)
{
    return !operator==(left,right);
}
