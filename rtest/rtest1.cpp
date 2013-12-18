#include "rfilename.h"
#include "rintegration.h"
#include "rlasterr.h"
#include <iostream>
using namespace std;
using namespace rtypes;

ostream& operator <<(ostream& stream,const path& p)
{
    stream << "Name: " << p.get_name() << endl
         << "Full Name: " << p.get_full_name() << endl
         << "Top Level Name: " << p.get_top_name() << endl
         << "Parent Path: " << p.get_parent_name() << endl
         << "Relative Name (work dir): " << p.get_relative_name();
    return stream;
}

int main(int argc,const char* argv[])
{
    if (argc < 3)
    {
        cerr << "Specify a command then a path string\n";
        return 1;
    }

    str command( argv[1] );
    path p( argv[2] );

    if (command == "make")
    {
        if ( p.exists() )
            cout << "The path " << p.get_full_name() << " already exists!\n";
        else
        {
            char makeSubDirs;
            cout << "Make sub-dirs? ";
            makeSubDirs = cin.get();
            if ( !p.make(makeSubDirs=='y' || makeSubDirs=='Y') )
                cerr << "An error occurred!\nReason: " << rlib_last_error::get()->message() << endl;
        }
    }
    else if (command == "remove")
    {
        if ( !p.remove() )
        {
            cout << "Could not remove directory! Reason: ";
            cout << rlib_last_error::get()->message() << endl;
        }
        else
            cout << "removed " << p.get_name() << endl;
    }
    else if (command == "erase")
    {
        char failOnSubDirs;
        cout << "Fail on sub-directories? ";
        failOnSubDirs = cin.get();
        if ( !p.erase(failOnSubDirs=='y' || failOnSubDirs=='Y') )
        {
            cout << "Could not erase directory! Reason: ";
            cout << rlib_last_error::get()->message() << endl;
        }
        else
            cout << "erased " << p.get_name() << endl;
    }
    else if (command == "list")
    {
        simple_listing files;
        if ( !p.get_complete_listing(files) )
        {
            cout << "Could not obtain listing! Reason: ";
            cout << rlib_last_error::get()->message() << endl;
        }
        else
        {
            for (dword i = 0;i<files.size();i++)
                cout << files[i] << endl;
        }
    }
    else if (command == "count")
    {
        int cnt = p.count_elements();
        if (cnt < 0)
        {
            cout << "The directory specified is incorrect.\n";
        }
        else
            cout << "The number of elements in the specified directory is " << cnt << endl;
    }
    else if (command == "resolve")
        cout << p.get_full_name() << endl;
    else if (command == "rename")
    {
        if (argc >= 4)
        {
            if ( !p.rename( argv[3]) )
            {
                cout << "Could not rename directory!\n";
                cout << "Reason: " << rlib_last_error::get()->message() << endl;
            }
            else
                cout << "Renamed directory.\n";
        }
        else
            cerr << "Specify another parameter\n";
    }
    else
    {
        cerr << "Bad command\n";
        return 2;
    }

    return 0;
}
