#include "rstreammanip.h"
using namespace rtypes;

// rtypes::_rstream_endflush_manipulator
void _rstream_endflush_manipulator::operation(rstream& stream) const
{
    stream.put(newline);
    if ( stream.does_buffer_output() )
        stream.flush_output();
}
const _rstream_endflush_manipulator rtypes::endline;

// rtypes::setw
setw::setw(word numericWidth)
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
