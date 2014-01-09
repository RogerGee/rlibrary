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
setw::setw(int numericWidth)
    : nwidth(numericWidth)
{
}
void setw::operation(rstream& stream) const
{
    stream.width(nwidth);
}
