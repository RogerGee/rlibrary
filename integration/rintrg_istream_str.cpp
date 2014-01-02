#include "../rintegration.h"
using namespace std;
using namespace rtypes;

istream& rtypes::operator >>(istream& stream,generic_string& s)
{
    istream::sentry sen(stream);
    if (sen)
    {
        s.clear();
        while ( stream.good() )
        {
            char c = stream.get();
            if ( isspace(c) )
                break;
            s.push_back(c);
        }
    }
    return stream;
}
