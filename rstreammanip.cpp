#include "rstreammanip.h"
using namespace rtypes;

// rtypes::_rstream_flush_manipulator
void _rstream_flush_manipulator::operation(rstream& stream) const
{
    if ( stream.does_buffer_output() )
        stream.flush_output();
}
void _rstream_flush_manipulator::operation(rbinstream& stream) const
{
    if ( stream.does_buffer_output() )
        stream.flush_output();
}
const _rstream_flush_manipulator rtypes::flush;

// rtypes::_rstream_endflush_manipulator
void _rstream_endflush_manipulator::operation(rstream& stream) const
{
    stream.put(newline);
    if ( stream.does_buffer_output() )
        stream.flush_output();
}
const _rstream_endflush_manipulator rtypes::endline;

// rtypes::setw
setw::setw(uint16 numericWidth)
    : nwidth(numericWidth)
{
}
void setw::operation(rstream& stream) const
{
    stream.width(nwidth);
}

// rtypes::setprecision
setprecision::setprecision(byte digitCnt)
    : ndigits(digitCnt)
{
}
void setprecision::operation(rstream& stream) const
{
    stream.precision(ndigits);
}
