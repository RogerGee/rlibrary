#include "rfilename.h"
#include "rintegration.h"
#include "rlasterr.h"
#include "riodevice.h"
#include <iostream>
using namespace std;
using namespace rtypes;

ostream& operator <<(ostream& stream,const path& p)
{
    stream << "name: " << p.get_name() << endl
           << "full_name: " << p.get_full_name() << endl
           << "top_name: " << p.get_top_name() << endl
           << "parent_name: " << p.get_parent_name() << endl
           << "relative_name_to_work_dir: " << p.get_relative_name();
    return stream;
}

ostream& operator <<(ostream& stream,const filename& fn)
{
    stream << "name: " << fn.get_name() << endl
           << "namex: " << fn.get_namex() << endl
           << "extension: " << fn.get_extension() << endl
           << "full_name: " << fn.get_full_name() << endl
           << "path: [\n" << fn.get_path() << "\n]" << endl
           << "relative_name: " << fn.get_relative_name();
    return stream;
}

int main()
{
    io_resource res;
    int fd = 0;

    res = fd;
}
