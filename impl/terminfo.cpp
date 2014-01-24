// terminfo.cpp
#include "terminfo.h"
#include "rstack.h"
#include "rstdio.h"
#include <unistd.h>
#include <stdlib.h>
#include "rstringstream.h"
#include "rfilename.h"
#include "rstdio.h"
// #include "rfile.h"
using namespace rtypes;
using namespace rtypes::rimpl;

namespace {
    // param_item for terminfo sequence interpretation
    struct param_item
    {
        param_item();
        param_item(int);
        param_item(const char* pstring);
        param_item(const str&);

        void init(int);
        void init(const char* pstring);
        void init(const str&);

        const generic_string& as_string() const
        { return param.get_device(); }
        int as_int() const;
    private:
        mutable rstringstream param;
    };

    param_item::param_item()
    {
    }
    param_item::param_item(int var)
    {
        // put the integer on the stream as a string
        param << var;
    }
    param_item::param_item(const char* pstring)
        : param(pstring)
    {
    }
    param_item::param_item(const str& s)
        : param( s.c_str() )
    {
    }
    void param_item::init(int var)
    {
        param.clear();
        param << var;
    }
    void param_item::init(const char* pstring)
    {
        param.open(pstring);
    }
    void param_item::init(const str& s)
    {
        param.open( s.c_str() );
    }
    int param_item::as_int() const
    {
        int i;
        param.set_input_iter(0);
        param >> i;
        return i;
    }

    str get_sequence_recursive(const_rstringstream& ss,stack<param_item>& st,const char* const* const parameters,int count)
    {
        /* this function recursively resolves parameterized terminfo strings; at the end of
         *  each frame, a value is pushed back that indicates whether or not the end of either
         *  a main or conditional context has been reached
         */
        int add = 0; /* how many times to "add 1" to each parameter pushed */
        bool cond = false; /* is this frame the original conditional context? */
        rstringstream result; /* store the result */
        while ( ss.has_input() )
        {
            char c = ss.get();
            if (c != '%')
            {
                result.put(c);
                continue;
            }
            c = ss.get();
            switch (c)
            {
            case 'p': // push parameter i onto stack
            {
                // parameters are indexed 1-9
                int index = ss.get() - '0' - 1;
                if (index>=0 && index<count && index<=9)
                {
                    param_item item( parameters[index] );
                    if (add > 0)
                    {
                        item.init(item.as_int() + 1);
                        --add;
                    }
                    st.push(item);
                }
                break;
            }
            case 'c':
            case 'd':
            case 's': // pop and print
            {
                param_item item = st.pop();
                if (c == 'c')
                    result << item.as_string().at(0);
                else if (c == 'd')
                    result << item.as_int();
                else
                    result << item.as_string();
                break;
            }
            case 'l': // pop and push size of string
            {
                param_item item = st.pop();
                st.push( item.as_string().length() );
                break;
            }
            case '\'': // push character (string?) literal
            {
                str item;
                ss.add_extra_delimiter('\'');
                ss >> item;
                ss.remove_extra_delimiter('\'');
                st.push(item);
                break;
            }
            case '{': // push integer literal
            {
                str item;
                ss.add_extra_delimiter('}');
                ss >> item;
                ss.remove_extra_delimiter('}');
                st.push(item);
                break;
            }
            case '+': // mathematical binary operators
            case '-':
            case '*':
            case '/':
            case 'm': // modular division
            case 'A': // logical AND
            case 'O': // logical OR
            case '&': // bitwise binary operations
            case '|':
            case '^':
            case '=': // logical comparison operations
            case '>':
            case '<':
            {
                int left, right;
                right = st.pop().as_int();
                left = st.pop().as_int();
                st.push( c=='+' ? left + right
                    : c=='-' ? left - right
                    : c=='*' ? left * right
                    : c=='/' ? left / right
                    : c=='m' ? left % right
                    : c=='A' ? (left && right ? 1 : 0)
                    : c=='O' ? (left || right ? 1 : 0)
                    : c=='&' ? left & right
                    : c=='|' ? left | right
                    : c=='^' ? left ^ right
                    : c=='=' ? (left == right ? 1 : 0)
                    : c=='>' ? (left > right ? 1 : 0)
                    : c=='<' ? (left < right ? 1 : 0) : 0 );
                break;
            }
            case '!': // (logical)
            case '~': // (bitwise)
            {
                // unary operations
                int op = st.pop().as_int();
                st.push( c=='!' ? (!op ? 1 : 0)
                    : c=='~' ? ~c : 0 );
                break;
            }
            case 'i':
                add = 2;
                break;
            case '?':
                cond = true;
                break;
            case 't': // "then" part
            {
                // recursively execute if-then-[else] construct
                str intRes;
                int sz;
                int condition;
                int finished;
                // read off the condition; it's assumed to have
                // been pushed up on the stack
                condition = st.pop().as_int();
                // get "then" part
                sz = st.size();
                intRes = get_sequence_recursive(ss,st,parameters,count);
                finished = st.pop().as_int();
                // test the condition
                if (condition)
                    result << intRes;
                if (!finished)
                {
                    if (!condition) // unload stack frame from "then" part
                        st.pop_range(st.size() - sz);
                    intRes = get_sequence_recursive(ss,st,parameters,count);
                    if (!condition)
                        result << intRes;
                    finished = st.pop().as_int();
                    if (!finished) // should be finished at this point
                        throw terminfo_sequence_eval_error();
                }
                if (!cond)
                {
                    st.push(1);
                    return str(result.get_device());
                }
                cond = false;
                break;
            }
            case 'e':
            case ';': // conditional segment terminators; semi means that the condition has been terminated
                st.push(c==';' ? 1 : 0);
                return str(result.get_device());
            }
        }
        st.push(1);
        return str(result.get_device());
    }
}

terminfo_sequence::terminfo_sequence(const str& source)
    : _sequence(source)
{
}
str terminfo_sequence::get_sequence(const char* const* const parameters,int count) const
{
    stack<param_item> st;
    const_rstringstream ss(_sequence);
    return get_sequence_recursive(ss,st,parameters,count);
}

terminfo::terminfo()
{
    const char* TERM = getenv("TERM");
    if (TERM != NULL)
    {
        str term, home;
        term = TERM;
        if (term.allocation_size() >= 2) // (should be)
        {
            const char* HOME = getenv("HOME");
            if (HOME != NULL)
            {
                home = HOME;
                home += "/.terminfo";
            }
            // check the following locations:
            const char* locations[] = {
                "/lib/terminfo",
                "/etc/terminfo",
                "/usr/share/terminfo",
                home.c_str()
            };
            for (short i = 0;i<4;i++)
            {
                char swap;
                path pcur(locations[i]);
                swap = term[1];
                term[1] = 0;
                pcur.append_name(term.c_str());
                term[1] = swap;
                if ( pcur.exists() )
                {
                    filename fnTerminfo(pcur,term);
                    if ( fnTerminfo.exists() )
                    {
                        //if ( !_read(bstream) )
                        //    throw terminfo_parse_error();
                        return;
                    }
                }
            }
        }
    }
    throw terminfo_environment_error();
}
bool terminfo::is_capable(term_boolean_cap) const
{
    return false;
}
bool terminfo::is_capable(term_numeric_cap) const
{
    return false;
}
bool terminfo::is_capable(term_string_cap) const
{
    return false;
}
bool terminfo::operator [](term_boolean_cap) const
{
    return false;
}
word terminfo::operator [](term_numeric_cap) const
{
    return 0;
}
terminfo_sequence terminfo::operator [](term_string_cap) const
{
    return terminfo_sequence("%'roger'%s");
}
bool terminfo::_read(rbinstream& bstream)
{
    word magic;
    word nameSize;
    word bools;
    word nums;
    word offs;
    word tblSize;
    // read the header
    bstream >> magic >> nameSize >> bools
            >> nums >> offs >> tblSize;
    if (magic == 0432)
    {
        bstream << binary_string_null_terminated;
        bstream >> _name;
        for (word i = 0;i<bools;i++)
            bstream >> ++_booleanCaps;
        // make sure that the next position is aligned
        // on a word boundery (even-byte boundry)
        if (bstream.get_input_iter() % 2 != 0)
            bstream.seek_input_iter(1);
        for (word i = 0;i<nums;i++)
            bstream >> ++_numericCaps;
        // seek the iterator to the string table
        bstream.seek_input_iter(offs*2);
        for (word i = 0;i<offs;i++)
            bstream >> ++_stringCaps;
        return true;
    }
    return false;
}
