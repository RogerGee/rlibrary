#include "../rintegration.h"
using namespace std;
using namespace rtypes;

// place the str into the std::ostream
ostream& rtypes::operator <<(ostream& stream,const str& s)
{
    stream << s.c_str();
    return stream;
}
