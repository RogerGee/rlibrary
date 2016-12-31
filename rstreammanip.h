/* rstreammanip.h -
 *  provides basic stream manipulators; stream manipulators
 * provide convinient access to member functions of an rstream
 * of rbinstream object
 */
#ifndef RSTREAMMANIP_H
#define RSTREAMMANIP_H
#include "rstream.h"

namespace rtypes
{
    /* rstream_manipulator
     *  represents the basic design of an
     * rstream manipulator which performs
     * a member-bound operation on an rstream
     */
    class rstream_manipulator
    {
    public:
        void op(rstream& stream) const
        { operation(stream); }
    private:
        virtual void operation(rstream& stream) const = 0;
    };

    /* rbinstream_manipulator
     *  represents the basic design of an rbinstream
     * manipulator which performs a member-bound
     * operation on an rbinstream
     */
    class rbinstream_manipulator
    {
    public:
        void op(rbinstream& stream) const
        { operation(stream); }
    private:
        virtual void operation(rbinstream& stream) const = 0;
    };

    // standard manipulators

    class _rstream_flush_manipulator : public rstream_manipulator,
                                       public rbinstream_manipulator
    {
    public:
        _rstream_flush_manipulator() {}
    private:
        virtual void operation(rstream& stream) const;
        virtual void operation(rbinstream& stream) const;
    };

    class _rstream_endflush_manipulator : public rstream_manipulator
    {
    public:
        _rstream_endflush_manipulator() {}
    private:
        virtual void operation(rstream& stream) const;
    };

    /* flush
     *  object that, when inserted into an rstream or rbinstream,
     * flushes the stream's output buffer; this only occurs if
     * the stream is set to buffer output locally
     */
    extern const _rstream_flush_manipulator flush;

    /* endline
     *  object that, when inserted into an rstream, inserts
     * rtypes::newline and, if the stream is buffering output
     * locally, flushes the output buffer
     */
    extern const _rstream_endflush_manipulator endline;

    class setw : public rstream_manipulator
    {
    public:
        setw(uint16 numericWidth);
    private:
        uint16 nwidth;

        virtual void operation(rstream& stream) const;
    };

    class setprecision : public rstream_manipulator
    {
    public:
        setprecision(byte digitCnt);
    private:
        byte ndigits;

        virtual void operation(rstream& stream) const;
    };
}

#endif
