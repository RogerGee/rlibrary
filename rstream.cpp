// rstream.cpp
#include "rstream.h"
#include "rstack.h"
#include "rstreammanip.h"
using namespace rtypes;

const char rtypes::newline = '\n';

stream_buffer::stream_buffer()
{
    _ideviceIter = 0;
    _odeviceIter = 0;
}
bool stream_buffer::_popInput(char& var)
{
    // sync input and output
    if ( !_bufOut.is_empty() )
        _outDevice();
    // attempt to read from local input buffer
    if (_hasInput())
    {
        var = _bufIn.pop();
        return true;
    }
    // attempt to get more input from device
    if ( _inDevice() ) // request more input
        return _popInput(var); // get next char from input
    //else no input was available from source
    return false;
}
bool stream_buffer::_peekInput(char& var) const
{
    // attempt to read from local input buffer
    if (_hasInput())
    {
        var = _bufIn.peek(); // doesn't advance the iterator
        return true;
    }
    // attempt to get more input from device
    if ( _inDevice() ) // check for input
        return _peekInput(var); // get next char from input
    //else no input was available from source
    return false;
}
bool stream_buffer::_peekInput(char& var,dword i) const
{
    // attempt to read from local input buffer
    if ( _hasInput() && _bufIn.size()>i )
    {
        var = *(&_bufIn.peek()+i);
        return true;
    }
    // attempt to get more input from device
    if ( _inDevice() ) // check for input
        return _peekInput(var,i); // get next char from input
    // else no input was available from source
    return false;
}
void stream_buffer::_pushBackOutputString(const char* pcstr)
{
    dword i = 0;
    while (pcstr[i])
        _bufOut.push( pcstr[i++] );
}
void stream_buffer::_pushBackOutputString(const generic_string& s)
{
    for (dword i = 0;i<s.size();i++)
        _bufOut.push(s[i]);
}

stream_base::stream_base()
{
    _lastSuccess = true; // "no operation" is considered a successful state; by default the stream's success state is good
    _doesBuffer = false;
}
char stream_base::get()
{
    char c = -1;
    _lastSuccess = _popInput(c);
    return c;
}
char stream_base::peek() const
{
    char c = -1;
    _lastSuccess = _peekInput(c);
    return c;
}
char stream_base::peek(dword i) const
{
    char c = -1;
    _lastSuccess = _peekInput(c,i);
    return c;
}
void stream_base::put(char c)
{
    _pushBackOutput(c);
    if (!_doesBuffer)
        _outDevice();
}
void stream_base::place(const stream_base& obj)
{
    // take all data from the stream and
    // place it into this stream; place only
    // buffered data
    dword len = obj._bufOut.size();
    const char* data = &obj._bufOut.peek();
    _bufOut.push_range(data,len);
}
void stream_base::set_input_iter(dword iter)
{
    int amount = int(iter) - int(get_input_iter());
    if (amount < 0)
    {
        _flushInputBuffer(); // flush because data is now invalidated
        _ideviceIter = iter;
    }
    else if (amount <= int(_bufIn.size()))
        _bufIn.pop_range(amount);
    else
    {
        amount -= _bufIn.size();
        _flushInputBuffer(); // pop past everything in input buffer
        _ideviceIter += amount;
    }
}
void stream_base::set_output_iter(dword iter)
{
    int amount = int(iter) - int(get_input_iter());
    if (amount < 0)
    {
        flush_output(); // any data assumed to have been intended for previous offset destination
        _odeviceIter = iter;
    }
    else if (amount <= int(_bufOut.size()))
        _bufOut.pop_range(amount);
    else
    {
        amount -= _bufOut.size();
        _flushOutputBuffer();
        _odeviceIter += amount;
    }
}
bool stream_base::has_input() const
{
    if ( !_hasInput() )
    {
        // there is no input in the stream input buffer
        char dummy;
        return _peekInput(dummy); // try to read a character to see if input available from device
    }
    return true; // input in stream input buffer
}

rstream::rstream()
{
    // initialize default manipulators
    _width = 0;
    _fill = ' ';
    _repFlag = decimal;
    _delimitWhitespace = true;
}
void rstream::add_extra_delimiter(char c)
{
    // add a new delimiter to the basic whitespace set
    _delimits.insert(c);
}
void rstream::add_extra_delimiter(const generic_string& delimiterString)
{
    // add multiple delimiters with one call
    for (dword i = 0;i<delimiterString.length();i++)
        _delimits.insert( delimiterString[i] );
}
void rstream::remove_extra_delimiter(char c)
{
    _delimits.remove(c);
}
void rstream::remove_extra_delimiter(const generic_string& delimiterString)
{
    // attempt to remove all delimiters in the string
    for (dword i = 0;i<delimiterString.length();i++)
        if ( _delimits.contains( delimiterString[i] ) )
            _delimits.remove( delimiterString[i] );
}
void rstream::clear_extra_delimiters()
{
    // reset delimiters to whitespace only
    _delimits.empty();
}
bool rstream::delimit_whitespace(bool yes)
{
    // turn off/on whitespace delimiters
    bool b = _delimitWhitespace;
    _delimitWhitespace = yes;
    return b;
}
dword rstream::width(dword wide)
{
    dword tmp = _width;
    _width = wide;
    return tmp;
}
char rstream::fill(char fillChar)
{
    char tmp = _fill;
    _fill = fillChar;
    return tmp;
}
void rstream::getline(generic_string& var)
{
    var.clear();
    char input;
    while (true)
    {
        if (!_popInput(input))
        {
            _lastSuccess = false;
            return;
        }
        else if (input=='\n')
        {
            _lastSuccess = true;
            return;
        }
        //else if (input=='\r') // newline encoding is not supposed to use this with stream buffer data
        //      continue;
        var.push_back(input);
    }
}
void rstream::putline(const generic_string& text)
{
    _pushBackOutputString(text);
    _pushBackOutput('\n'); //add a newline
    if (!_doesBuffer)
        _outDevice();
}
rstream& rstream::operator >>(bool& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        if (input>='A' && input<='Z')
        {// force lowercase
            input -= 'A';
            input += 'a';
        }
        s.push_back(input);
    }
    for (dword i = 0;i<s.length();i++)
        if (s[i]>='A' && s[i]<='Z')
            s[i] -= 'A', s[i] += 'a';
    _lastSuccess = true;
    if (s=="true")
        var = true;
    else if (s=="false")
        var = false;
    else
        _lastSuccess = false;
    return *this;   
}
rstream& rstream::operator >>(char& var)
{
    char input;
    do
    {
        if ( !_popInput(input) )
        {
            _lastSuccess = false;
            return *this; // no available input
        }
    } while (_isWhitespace(input));
    var = input;
    _lastSuccess = true;
    return *this;
}
rstream& rstream::operator >>(byte& var)
{
    char input;
    do
    {
        if ( !_popInput(input) )
        {
            _lastSuccess = false;
            return *this; // no available input
        }
    } while (_isWhitespace(input));
    var = input;
    _lastSuccess = true;
    return *this;
}
rstream& rstream::operator >>(short& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<short> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(word& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<word> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(int& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<int> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(dword& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<dword> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(long& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<long> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(unsigned long& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<unsigned long> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(long long& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<long long> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(qword& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = _fromString<qword> (s,_lastSuccess);
    return *this;
}
rstream& rstream::operator >>(double& var)
{
    // unimplemented
    var = 0.0;
    _lastSuccess = false;
    return *this;
}
rstream& rstream::operator >>(void*& var)
{
    str s;
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (s.size()>0)
                break;
            else
                continue;
        }
        s.push_back(input);
    }
    if (!s.size())
    {
        _lastSuccess = false;
        return *this;
    }
    var = reinterpret_cast<void*> (_fromString<dword> (s,_lastSuccess));
    return *this;
}
rstream& rstream::operator >>(generic_string& var)
{
    var.clear(); // overwrite var
    char input;
    while (true)
    {
        if (!_popInput(input))
            break;
        else if (_isWhitespace(input))
        {
            if (var.size()>0)
                break;
            else
                continue;
        }
        var.push_back(input);
    }
    _lastSuccess = var.size()>0; // success if any characters were read
    return *this;
}
rstream& rstream::operator <<(bool b)
{
    _pushBackOutputString(b ? "true" : "false");
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(char c)
{
    _pushBackOutput(c);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(byte b)
{
    _pushBackOutput(b);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(short s)
{
    _pushBackNumeric(s,s<0);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(word w)
{
    _pushBackNumeric(w,false);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(int i)
{
    _pushBackNumeric(i,i<0);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(dword d)
{
    _pushBackNumeric(d,false);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(long l)
{
    _pushBackNumeric(l,l<0);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(unsigned long ul)
{
    _pushBackNumeric(ul,false);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const long long& l)
{
    _pushBackNumeric(l,l<0);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const qword& q)
{
    _pushBackNumeric(q,false);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const double& d)
{
    const qword* data = reinterpret_cast<const qword*> (&d);
    qword var, sig;
    short expn;
    bool sign;
    stack<char> digits;
    sign = (*data>>63) == 1;
    expn = short(((*data>>56)&0x7f)<<4) | (((*data>>48)&0xff)>>4);
    sig = *data & 0xfffffffffffff;
    // special cases
    if (expn==0x000)
    {
        if (sig==0)
        {
            _pushBackOutputString(sign ? "-0.0" : "0.0");
            return *this;
        }
    }
    else if (expn==0x7ff)
    {
        if (sig==0)
            _pushBackOutputString(sign ? "-infinity" : "infinity");
        else
            _pushBackOutputString("NaN");
        return *this;
    }
    // apply implicit 1 (hidden bit) to significand if not subnormal
    if (expn != 0x000)
        sig |= qword(1)<<52;
    // apply the exponent bias
    expn -= 1023;
    // output
    if (sign)
        _pushBackOutput('-');
    if ((expn<0 ? ~expn+1 : expn) <= 17)
    {
        // use x.x decimal format (because we have powers of ten large enough to compute)
        if (expn >= 0)
        {
            _pushBackNumeric(sig>>(52-expn),false);
            _pushBackOutput('.');
            sig <<= expn;
        }
        else
        {
            _pushBackOutputString("0.");
            sig >>= _abs(expn);
        }
        var = 1;
        for (short i = 1;i<=18;i++)
        {
            var *= 10;
            if ((sig>>(52-i))&1)
            {
                qword powerOfTen = _pow(qword(10),qword(i));
                qword powerOfTwo = 1 << i;
                var += powerOfTen / powerOfTwo;
            }
        }
        // get digits
        while (var != 0)
        {
            char digit = var % qword(10);
            if (digit!=0/*trailing zeros*/ || digits.count()>0)
                digits.push('0'+digit);
            var /= 10;
        }
        digits.pop(); // most sig. _repBase digit
        // push back digits
        while ( !digits.is_empty() )
            _pushBackOutput( digits.pop() );
    }
    return *this;
}
rstream& rstream::operator <<(const void* p)
{
    numeric_representation nFlag = _repFlag;
    _repFlag = hexadecimal;
    _pushBackNumeric<qword>(reinterpret_cast<qword>(p),false,"0x");
    _repFlag = nFlag;
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const char* cs)
{
    dword i = 0;
    _pushBackOutputString(cs);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const generic_string& s)
{
    _pushBackOutputString(s);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(numeric_representation flag)
{
    _repFlag = flag;
    return *this;
}
rstream& rstream::operator <<(const rstream_manipulator& manipulator)
{
    manipulator.op(*this);
    return *this;
}
bool rstream::_isWhitespace(char c)
{
    if ( (_delimitWhitespace && (c==' ' || c=='\n' || c=='\t')) || c<=0 /* fail state for text streams (at end of stream) */ || _delimits.contains(c) )
    {
        _delimStrActive.push_back(c);
        return true;
    }
    else if ( _delimStrActive.size()>0 )
    {
        _delimStrLast = _delimStrActive;
        _delimStrActive.clear();
    }
    return false;
}
template<class Numeric>
void rstream::_pushBackNumeric(Numeric n,bool is_neg,const char* prefix)
{
    if (is_neg && _repFlag!=decimal)
    {// occurs for signed types only - this preserves byte values for non-decimal reps
        qword mask = 0; // will mask bits not needed by the size of Numeric
        qword q = n; // treat n (Numeric) as an unsigned value
        for (dword i = 0;i<sizeof(Numeric);i++)
        {
            mask <<= 8;
            mask |= 0xff;
        }
        q &= mask; // apply mask
        _pushBackNumeric(q,false);// recursively call _toString to get negative rep for non-decimal rep number
        return;
    }
    stack<char> digitChars;
    str r;
    if (prefix != NULL)
        r = prefix;
    if (is_neg) // only true if Numeric is a signed type
        r.push_back('-');
    else if (n==0)
        digitChars.push('0');
    if (_repFlag>1)
    {
        while (n!=0)
        {
            char off = (char) (n%(Numeric) _repFlag);
            if (_abs(off)<=9)
                digitChars.push('0'+off);
            else
                digitChars.push('A'+(off-10));
            n /= (Numeric) _repFlag;
        }
    }
    else if (_repFlag==1)
    {// admit it, base-1 is hilarious!
        while (n!=0) // n will always be of an unsigned type if using base-1
        {
            digitChars.push('0');
            n -= 1;
        }
    }
    if (_width > digitChars.size()+r.size())
    {
        for (dword i = 0;i<digitChars.size()+r.size()-_width;i++)
            r.push_back(_fill);
    }
    while ( !digitChars.is_empty() )
        r.push_back( digitChars.pop() );
    _pushBackOutputString(r);
}
template<class Numeric>
Numeric rstream::_fromString(const str& s/* will have at least 1 char */,bool& success)
{
    Numeric r = 0;
    bool isNeg = (s[0]=='-' && s.size()>1);
    // calculate inclusive bounds on numeric representation characters
    // for different numeric bases; supported are 0-9, A-Z, and a-z depending
    // on the base
    const char arabicBound = _repFlag<=decimal ? '9' : '0'+char(_repFlag-1);
    const char ucaseLetterBound = _repFlag>decimal ? 'A'+char(_repFlag-1) : -1;
    const char lcaseLetterBound = _repFlag>decimal ? 'a'+char(_repFlag-1) : -1;
    // go through each character; stop prematurely on bad character
    for (dword i = (isNeg || (s[0]=='+' && s.size()>1) ? 1 : 0);i<s.size();i++)
    {
        char digi = -1;
        if (s[i]>='0' && s[i]<=arabicBound)
            digi = s[i]-'0';
        if (ucaseLetterBound != -1) // support bases larger than 'decimal'
        {
            if (s[i]>='A' && s[i]<=ucaseLetterBound)
                digi = s[i]-'A';
            else if (s[i]>='a' && s[i]<=lcaseLetterBound)
                digi = s[i]-'a';
        }
        if (digi == -1) // bad character encountered
        {
            // try to preserve any good value before
            // a bad character
            if (isNeg)
                r *= -1;
            success = false;
            return r;
        }
        r *= (Numeric) _repFlag;
        r += digi;
    }
    if (isNeg)
        r *= -1;
    success = true;
    return r;
}
template<class Numeric>
/* static */ Numeric rstream::_abs(Numeric& n)
{
    if (n<0)
        n = ~n + 1;
    return n;
}
template<class Numeric>
/* static */ Numeric rstream::_pow(Numeric x,Numeric y)
{
    if (y==0)
        return 1;
    else if (y==1)
        return x;
    return x*_pow(x,--y);
}

rbinstream::rbinstream()
{
    _endianFlag = little;
}
rbinstream::rbinstream(endianness e)
{
    _endianFlag = e;
}

rbinstream& rbinstream::operator >>(bool& var)
{
    // non-zero is true; zero is false
    char in;
    _lastSuccess = _popInput(in);
    if (_lastSuccess)
        var = in!=0;
    return *this;
}
rbinstream& rbinstream::operator >>(char& var)
{
    _lastSuccess = _popInput(var);
    return *this;
}
rbinstream& rbinstream::operator >>(byte& var)
{
    _lastSuccess = _popInput((char&) var);
    return *this;
}
rbinstream& rbinstream::operator >>(short& var)
{
    str data(sizeof(short));
    // load data
    for (dword i = 0;i<sizeof(short);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<short>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(word& var)
{
    str data(sizeof(word));
    // load data
    for (dword i = 0;i<sizeof(word);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<word>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(int& var)
{
    str data(sizeof(int));
    // load data
    for (dword i = 0;i<sizeof(int);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<int>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(dword& var)
{
    str data(sizeof(dword));
    // load data
    for (dword i = 0;i<sizeof(dword);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<dword>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(long& var)
{
    str data(sizeof(long));
    // load data
    for (dword i = 0;i<sizeof(long);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<long>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(unsigned long& var)
{
    str data(sizeof(unsigned long));
    // load data
    for (dword i = 0;i<sizeof(unsigned long);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<unsigned long>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(long long& var)
{
    str data(sizeof(long long));
    // load data
    for (dword i = 0;i<sizeof(long long);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<long long>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(qword& var)
{
    str data(sizeof(qword));
    // load data
    for (dword i = 0;i<sizeof(qword);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = _fromString<qword>(data,_lastSuccess);
    return *this;
}
rbinstream& rbinstream::operator >>(double& var)
{
    str data( sizeof(double) );
    // load data
    for (dword i = 0;i<sizeof(double);i++)
    {
        if ( !_popInput(data[i]) )
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    qword rep = _fromString<qword>(data,_lastSuccess);
    var = *reinterpret_cast<double*> (&rep);
    return *this;
}
rbinstream& rbinstream::operator >>(void*& var)
{
    str data(sizeof(void*));
    // load data
    for (dword i = 0;i<sizeof(void*);i++)
    {
        if (!_popInput(data[i]))
        {
            _lastSuccess = false;
            return *this;
        }
    }
    // calc. value
    var = reinterpret_cast<void*> (_fromString<dword>(data,_lastSuccess));
    return *this;
}
rbinstream& rbinstream::operator >>(generic_string& var)
{
    // read the capacity of the string object
    dword i;
    for (i = 0;i<var.capacity();i++)
    {
        if (!_popInput(var[i]))
        {
            _lastSuccess = false;
            var.resize(i); // let the string be representative of what was read
            return *this;
        }
    }
    var.resize(i);
    _lastSuccess = true;
    return *this;
}
rbinstream& rbinstream::operator <<(bool b)
{
    _pushBackOutput(b ? 1 : 0);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(char c)
{
    _pushBackOutput(c);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(byte b)
{
    _pushBackOutput(b);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(short s)
{
    _pushBackBinaryOrder(s);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(word w)
{
    _pushBackBinaryOrder(w);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(int i)
{
    _pushBackBinaryOrder(i);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(dword d)
{
    _pushBackBinaryOrder(d);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(long l)
{
    _pushBackBinaryOrder(l);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(unsigned long ul)
{
    _pushBackBinaryOrder(ul);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const long long& l)
{
    _pushBackBinaryOrder(l);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const qword& q)
{
    _pushBackBinaryOrder(q);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const double& d)
{
    _pushBackBinaryOrder( *reinterpret_cast<const qword*>(&d) );
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const void* p)
{
    _pushBackBinaryOrder<qword>( reinterpret_cast<qword>(p) );
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const char* cs)
{
    dword i = 0;
    while ( cs[i] )
        _pushBackOutput( cs[i++] );
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const generic_string& s)
{
    _pushBackOutputString(s);
    if (!_doesBuffer)
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(endianness flag)
{
    _endianFlag = flag;
    return *this;
}
template<class Numeric>
void rbinstream::_pushBackBinaryOrder(Numeric n)
{
    // (assume endianness from _endianFlag)
    // divide n into its bytes and put into stream
    int byteCnt = sizeof(Numeric);
    if (_endianFlag==big)
    {// get most significant to least significant
        for (int i = byteCnt-1;i>=0;i--)
            _pushBackOutput( (n>>(8*i))&0xff );
    }
    else if (_endianFlag==little)
    {// get least significant to most significant
        for (int i = 0;i<byteCnt;i++)
            _pushBackOutput( (n>>(8*i))&0xff );
    }
}
template<class Numeric>
Numeric rbinstream::_fromString(const str& s,bool& success)
{
    // (assume endianness from _endianFlag)
    Numeric val = 0;
    dword byteCnt = sizeof(Numeric);
    if (s.size()!=byteCnt)
    {
        success = false;
        return val;
    }
    if (_endianFlag==big)
    {// bytes are ordered most significant to least significant
        for (int i = int(byteCnt)-1;i>=0;i--)
            val |= (((Numeric) s[int(byteCnt)-1-i] & 0xff)<<(8*i));
        success = true;
    }
    else if (_endianFlag==little)
    {// bytes are ordered least significant to most significant
        for (int i = 0;i<int(byteCnt);i++)
            val |= (((Numeric) s[i] & 0xff)<<(8*i));
        success = true;
    }
    else
        success = false;
    return val;
}
