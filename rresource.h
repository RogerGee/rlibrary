/* rresource.h
 *  rlibrary/rresource - provides a cross-platform
 * mechanism for representing a system resource
 * identifier
 */
#ifndef RRESOURCE_H
#define RRESOURCE_H
#include "rtypestypes.h"
#include <cstddef> // get size_t
#include "rerror.h"

namespace rtypes
{
    template<size_t BYTE_CNT>
    class resource
    {
        template<size_t BYTE_CNT2> 
        friend bool operator ==(const resource<BYTE_CNT2>&,const resource<BYTE_CNT2>&);
        template<size_t BYTE_CNT2> 
        friend bool operator !=(const resource<BYTE_CNT2>&,const resource<BYTE_CNT2>&);
    public:
        resource();
        resource(byte fillValue);
        resource(void* defaultValue);
        virtual ~resource() {}

        template<typename T>
        T& interpret_as()
        { return reinterpret_cast<T&>(_idData); }

        template<typename T>
        const T& interpret_as() const
        { return reinterpret_cast<const T&>(_idData); }

        template<typename T>
        resource& assign(const T&);
    private:
        // disallow copying
        resource(const resource&);
        resource& operator =(const resource&);

        byte _idData[BYTE_CNT];
    };

    template<size_t BYTE_CNT>
    bool operator ==(const resource<BYTE_CNT>&,const resource<BYTE_CNT>&);
    template<size_t BYTE_CNT>
    bool operator !=(const resource<BYTE_CNT>&,const resource<BYTE_CNT>&);
}

// include template implementation
#include "rresource.tcc"

#endif
