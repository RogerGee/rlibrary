/* rstreammanip.h -
 *  provides basic stream manipulators
 */
#ifndef RSTREAMMANIP_H
#define RSTREAMMANIP_H
#include "rstream.h"

namespace rtypes
{
    class rstream_manipulator
    {
    public:
        void op(rstream& stream) const
        { operation(stream); }
    private:
        virtual void operation(rstream& stream) const = 0;
    };

    class rbinstream_manipulator
    {
    public:
        void op(rbinstream& stream) const
        { operation(stream); }
    private:
        virtual void operation(rbinstream& stream) const = 0;
    };

    // standard manipulators
    class _rstream_endflush_manipulator : public rstream_manipulator
    {
        virtual void operation(rstream& stream) const;
    };
    extern const _rstream_endflush_manipulator endline;

    class setw : public rstream_manipulator
    {
    public:
        setw(int numericWidth);
    private:
        int nwidth;

        virtual void operation(rstream& stream) const;
    };
}

#endif
