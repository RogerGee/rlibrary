/* rresource.h
 *  rlibrary/rresource - provides a cross-platform
 * mechanism for representing a system resource
 * identifier
 */
#ifndef RRESOURCE_H
#define RRESOURCE_H
#include "rtypestypes.h"

namespace rtypes
{
    template<int BYTE_CNT>
    class resource
    {
    public:
        resource();
        virtual ~resource() {}

        template<typename T>
        T& reinterpret_as()
        { return reinterpret_cast<T&>(_idData); }

        template<typename T>
        const T& reinterpret_as() const
        { return reinterpret_cast<const T&>(_idData); }
    private:
        byte _idData[BYTE_CNT];
    };
}

template<int BYTE_CNT>
resource::resource()
{
    for (int i = 0;i<BYTE_CNT;i++)
        _idData[i] = 0;
}

#endif
