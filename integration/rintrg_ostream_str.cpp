#include "../rintegration.h"
using namespace std;
using namespace rtypes;

// place the str into the std::ostream
ostream& rtypes::operator <<(ostream& stream,const generic_string& s)
{
    stream << s.c_str();
    return stream;
}
