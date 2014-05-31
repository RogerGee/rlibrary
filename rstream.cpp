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
bool stream_buffer::_peekInput(char& var,size_type i) const
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
    size_type i = 0;
    while (pcstr[i])
        _bufOut.push( pcstr[i++] );
}
void stream_buffer::_pushBackOutputString(const generic_string& s)
{
    for (size_type i = 0;i<s.size();i++)
        _bufOut.push(s[i]);
}

stream_base::stream_base()
{
    _lastSuccess = true; // "no operation" is considered a successful state; by default the stream's success state is good
    _doesBuffer = true; // let the stream by default buffer output until a flush operation is executed
}
stream_base::stream_base(bool doesBuffer)
{
    _lastSuccess = true;
    _doesBuffer = doesBuffer;
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
char stream_base::peek(size_type i) const
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
    uint32 len = obj._bufOut.size();
    const char* data = &obj._bufOut.peek();
    _bufOut.push_range(data,len);
}
void stream_base::repeat(char c,uint32 times)
{
    for (uint32 i = 0;i<times;i++)
        _bufOut.push(c);
}
void stream_base::set_input_iter(size_type iter)
{
    int64 amount = int64(iter) - int64(get_input_iter());
    if (amount < 0)
    {
        _flushInputBuffer(); // flush because data is now invalidated
        _ideviceIter = iter;
    }
    else if (amount <= int64(_bufIn.size()))
        _bufIn.pop_range(amount);
    else
    {
        amount -= _bufIn.size(); // pop past everything in input buffer
        _flushInputBuffer();
        _ideviceIter += amount;
    }
}
void stream_base::set_output_iter(size_type iter)
{
    int64 amount = int64(iter) - int64(get_output_iter());
    if (amount < 0)
    {
        flush_output(); // any data assumed to have been intended for previous offset destination
        _odeviceIter = iter;
    }
    else if (amount <= int64(_bufOut.size()))
        _bufOut.pop_range(amount);
    else
    {
        amount -= _bufOut.size();
        _flushOutputBuffer(); // data is not written to device
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
    // call initializer member function
    _init();
}
rstream::rstream(bool doesBuffer)
    : stream_base(doesBuffer)
{
    // call initializer member function
    _init();
}
void rstream::add_extra_delimiter(char c)
{
    // add a new delimiter to the basic whitespace set
    _delimits.insert(c);
}
void rstream::add_extra_delimiter(const char* pdelims)
{
    size_type i = 0;
    while (pdelims[i])
        _delimits.insert( pdelims[i++] );
}
void rstream::add_extra_delimiter(const generic_string& delimiterString)
{
    // add multiple delimiters with one call
    for (size_type i = 0;i<delimiterString.length();i++)
        _delimits.insert( delimiterString[i] );
}
void rstream::remove_extra_delimiter(char c)
{
    _delimits.remove(c);
}
void rstream::remove_extra_delimiter(const char* pdelims)
{
    size_type i = 0;
    while (pdelims[i])
        _delimits.remove( pdelims[i++] );
}
void rstream::remove_extra_delimiter(const generic_string& delimiterString)
{
    // attempt to remove all delimiters in the string
    for (uint32 i = 0;i<delimiterString.length();i++)
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
uint16 rstream::width(uint16 wide)
{
    uint16 tmp = _width;
    _width = wide;
    return tmp;
}
byte rstream::precision(byte ndigits)
{
    byte tmp = _precision;
    _precision = ndigits;
    return tmp;
}
char rstream::fill(char fillChar)
{
    char tmp = _fill;
    _fill = fillChar;
    return tmp;
}
numeric_representation rstream::representation(numeric_representation rep)
{
    numeric_representation tmp = _repFlag;
    _repFlag = rep;
    return tmp;
}
void rstream::getline(generic_string& var)
{
    var.clear();
    char input;
    while (true)
    {
        if ( !_popInput(input) )
        {
            set_input_success(var.length() > 0);
            return;
        }
        else if (input == '\n')
        {
            set_input_success(true);
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
    if ( !does_buffer_output() )
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
    for (size_type i = 0;i<s.length();i++)
        if (s[i]>='A' && s[i]<='Z')
            s[i] -= 'A', s[i] += 'a';
    set_input_success(true);
    if (s=="true")
        var = true;
    else if (s=="false")
        var = false;
    else
        set_input_success(false);
    return *this;   
}
rstream& rstream::operator >>(char& var)
{
    char input;
    do
    {
        if ( !_popInput(input) )
        {
            set_input_success(false);
            return *this; // no available input
        }
    } while (_isWhitespace(input));
    var = input;
    set_input_success(true);
    return *this;
}
rstream& rstream::operator >>(byte& var)
{
    char input;
    do
    {
        if ( !_popInput(input) )
        {
            set_input_success(false);
            return *this; // no available input
        }
    } while (_isWhitespace(input));
    var = input;
    set_input_success(true);
    return *this;
}
rstream& rstream::operator >>(short& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<short> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(uint16& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<uint16> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(int& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<int> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(uint32& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<uint32> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(long& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<long> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(unsigned long& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<unsigned long> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(int64& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<int64> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(uint64& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = _fromString<uint64> (s,success);
    set_input_success(success);
    return *this;
}
rstream& rstream::operator >>(float&)
{
    /* unimplemented */
    set_input_success(false);
    return *this;
}
rstream& rstream::operator >>(double&)
{
    // unimplemented
    set_input_success(false);
    return *this;
}
rstream& rstream::operator >>(void*& var)
{
    str s;
    char input;
    bool success;
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
        set_input_success(false);
        return *this;
    }
    var = reinterpret_cast<void*> (_fromString<uint32> (s,success));
    set_input_success(success);
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
    set_input_success(var.size() > 0); // success if any characters were read
    return *this;
}
rstream& rstream::operator <<(bool b)
{
    _pushBackOutputString(b ? "true" : "false");
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(char c)
{
    _pushBackOutput(c);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(byte b)
{
    _pushBackOutput(b);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(short s)
{
    _pushBackNumeric(s,s<0);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(uint16 w)
{
    _pushBackNumeric(w,false);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(int i)
{
    _pushBackNumeric(i,i<0);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(uint32 d)
{
    _pushBackNumeric(d,false);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(long l)
{
    _pushBackNumeric(l,l<0);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(unsigned long ul)
{
    _pushBackNumeric(ul,false);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const int64& l)
{
    _pushBackNumeric(l,l<0);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const uint64& q)
{
    _pushBackNumeric(q,false);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(float)
{
    /* unimplemented */
    return *this;
}
rstream& rstream::operator <<(const double& d)
{
    // interpret the double as an integer of 64 bits
    const uint64* data = reinterpret_cast<const uint64*> (&d);
    // constants
    const short EXPONENT_BIAS = 1023;
    const short MAX_BASE10_POWER = 18;
    // locals
    str rep;
    uint64 sig;
    short expn;
    bool sign;
    // obtain the different parts of the double-precision floating point number
    sign = (*data>>63) == 1; // sign part
    expn = short(((*data>>56)&0x7f)<<4) | (((*data>>48)&0xff)>>4); // exponent part
    sig = *data & 0xfffffffffffff; // fraction part
    // special cases
    if (expn==0x000 && sig==0)
        rep.append(sign ? "-0.0" : "0.0");
    else if (expn==0x7ff && sig==0)
        rep.append(sign ? "-infinity" : "infinity");
    else if (expn==0x7ff && sig!=0)
        rep.append("NaN");
    // usual case
    else
    {
        // apply implicit 1 to significand if the number is normalized
        if (expn != 0x000)
            sig |= uint64(1)<<52;
        // apply the exponent bias (excess)
        expn -= EXPONENT_BIAS;
        // add prefixes to string
        if (sign)
            rep.push_back('-');
        // perform conversion:
        if ((expn>=0?expn:~expn+1) <= MAX_BASE10_POWER) // use x.x decimal format
        {
            // calculate the integer part and add it to the string
            rep.append( _convertNumeric(sig >> (52-expn)) );
            // move binary point left or right to normalize the number for its decimal
            // representation given the previously calculated integer part
            if (expn >= 0)
                sig <<= expn;
            else
                sig >>= _abs(expn);
            // calculate the fraction part
            uint64 var = 1;
            for (short i = 1;i<=MAX_BASE10_POWER;i++)
            {
                var *= 10;
                if ((sig>>(52-i)) & 1)
                {
                    uint64 powerOfTen = _pow(uint64(10),uint64(i));
                    uint64 powerOfTwo = 1 << i;
                    var += powerOfTen / powerOfTwo;
                }
            }
            // get decimal digits
            stack<char> digits;
            while (var != 0)
            {
                char digit = var % uint64(10);
                if (digit!=0/*ignore trailing zeros*/ || digits.count()>0)
                    digits.push('0'+digit);
                var /= 10;
            }
            digits.pop(); // most sig. _repBase digit
            // add decimal point to string
            rep.push_back('.');
            // add digits to string
            while ( !digits.is_empty() )
                rep.push_back( digits.pop() );
        }
        else // use scientific notation to express the floating-point number
        {
            /* unimplemented */
            
        }
    }
    _pushBackOutputString(rep);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const void* p)
{
    numeric_representation nFlag = _repFlag;
    _repFlag = hexadecimal;
    _pushBackNumeric<uint64>(reinterpret_cast<uint64>(p),false,"0x");
    _repFlag = nFlag;
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const char* cs)
{
    _pushBackOutputString(cs);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rstream& rstream::operator <<(const generic_string& s)
{
    _pushBackOutputString(s);
    if ( !does_buffer_output() )
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
void rstream::_init()
{
    // Initialize the rstream object:
    // initialize default manipulators
    _width = 0;
    _precision = 0;
    _fill = ' ';
    _repFlag = decimal;
    _delimitWhitespace = true;
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
void rstream::_pushBackNumeric(Numeric n,bool isNeg,const char* prefix)
{
    if (isNeg && _repFlag!=decimal) // occurs for signed types only - this preserves byte values for non-decimal reps
    {
        uint64 mask = 0; // will mask bits not needed by the size of Numeric
        uint64 q = n; // treat n (Numeric) as an unsigned value
        for (size_type i = 0;i<sizeof(Numeric);i++)
        {
            mask <<= 8;
            mask |= 0xff;
        }
        q &= mask; // apply mask
        _pushBackNumeric(q,false);// recursively call _toString to get negative rep for non-decimal rep number
        return;
    }
    size_type sz;
    str conv, result;
    // assign prefixes
    if (prefix != NULL)
        result = prefix;
    if (isNeg) // only true if Numeric is a signed type
        result.push_back('-');
    // obtain converted string representation
    conv = _convertNumeric(n);
    // add fill chars before digits
    sz = conv.size() + result.size();
    if (_width > sz)
    {
        sz = _width - sz;
        for (size_type i = 0;i<sz;i++)
            result.push_back(_fill);
    }
    // add converted numeric integer
    result += conv;
    // insert the result into the stream
    _pushBackOutputString(result);
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
    for (size_type i = (isNeg || (s[0]=='+' && s.size()>1) ? 1 : 0);i<s.size();i++)
    {
        char digi = -1;
        if (s[i]>='0' && s[i]<=arabicBound)
            digi = s[i]-'0';
        if (ucaseLetterBound != -1) // support bases larger than 'decimal'
        {
            if (s[i]>='A' && s[i]<=ucaseLetterBound)
                digi = s[i]-'A'+10;
            else if (s[i]>='a' && s[i]<=lcaseLetterBound)
                digi = s[i]-'a'+10;
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
        r = ~r + 1;
    success = true;
    return r;
}
template<class Numeric>
str rstream::_convertNumeric(Numeric n) const
{
    stack<char> digitChars;
    str r;
    // check for zero-case
    if (n == 0)
        digitChars.push('0');
    // convert numeric value into string representation
    // based on the current numeric-base flag
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
    // compile digits in the correct order
    while ( !digitChars.is_empty() )
        r.push_back( digitChars.pop() );
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
    _stringInputFormat = binary_string_capacity;
}
rbinstream::rbinstream(endianness e)
{
    _endianFlag = e;
    _stringInputFormat = binary_string_capacity;
}
rbinstream::rbinstream(bool doesBuffer)
    : stream_base(doesBuffer)
{
    _endianFlag = little;
    _stringInputFormat = binary_string_capacity;
}
rbinstream::rbinstream(endianness e,bool doesBuffer)
    : stream_base(doesBuffer)
{
    _endianFlag = e;
    _stringInputFormat = binary_string_capacity;
}
rbinstream& rbinstream::operator >>(bool& var)
{
    // non-zero is true; zero is false
    char in;
    bool success;
    success = _popInput(in);
    if (success)
        var = in!=0;
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(char& var)
{
    set_input_success( _popInput(var) );
    return *this;
}
rbinstream& rbinstream::operator >>(byte& var)
{
    set_input_success( _popInput((char&) var) );
    return *this;
}
rbinstream& rbinstream::operator >>(short& var)
{
    bool success;
    str data(sizeof(short));
    // load data
    for (size_type i = 0;i<sizeof(short);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<short>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(uint16& var)
{
    bool success;
    str data(sizeof(uint16));
    // load data
    for (size_type i = 0;i<sizeof(uint16);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<uint16>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(int& var)
{
    bool success;
    str data(sizeof(int));
    // load data
    for (size_type i = 0;i<sizeof(int);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<int>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(uint32& var)
{
    bool success;
    str data(sizeof(uint32));
    // load data
    for (size_type i = 0;i<sizeof(uint32);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<uint32>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(long& var)
{
    bool success;
    str data(sizeof(long));
    // load data
    for (size_type i = 0;i<sizeof(long);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<long>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(unsigned long& var)
{
    bool success;
    str data(sizeof(unsigned long));
    // load data
    for (size_type i = 0;i<sizeof(unsigned long);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<unsigned long>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(int64& var)
{
    bool success;
    str data(sizeof(int64));
    // load data
    for (size_type i = 0;i<sizeof(int64);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<int64>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(uint64& var)
{
    bool success;
    str data(sizeof(uint64));
    // load data
    for (size_type i = 0;i<sizeof(uint64);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = _fromString<uint64>(data,success);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(double& var)
{
    bool success;
    str data( sizeof(double) );
    // load data
    for (size_type i = 0;i<sizeof(double);i++)
    {
        if ( !_popInput(data[i]) )
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    uint64 rep = _fromString<uint64>(data,success);
    var = *reinterpret_cast<double*> (&rep);
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(void*& var)
{
    bool success;
    str data(sizeof(void*));
    // load data
    for (size_type i = 0;i<sizeof(void*);i++)
    {
        if (!_popInput(data[i]))
        {
            set_input_success(false);
            return *this;
        }
    }
    // calc. value
    var = reinterpret_cast<void*> (_fromString<uint32>(data,success));
    set_input_success(success);
    return *this;
}
rbinstream& rbinstream::operator >>(generic_string& var)
{
    // read based on the string input format
    if (_stringInputFormat == binary_string_capacity)
    {
        size_type i;
        for (i = 0;i<var.capacity();i++)
        {
            if (!_popInput(var[i]))
            {
                set_input_success(false);
                var.resize(i); // let the string be representative of what was read
                return *this;
            }
        }
        var.resize(i);
        set_input_success(var.size() == var.capacity());
    }
    else if (_stringInputFormat == binary_string_null_terminated)
    {
        char c = -1;
        while (_popInput(c) && c!=0)
            var.push_back(c);
        set_input_success( c==0 );
    }
    return *this;
}
rbinstream& rbinstream::operator <<(bool b)
{
    _pushBackOutput(b ? 1 : 0);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(char c)
{
    _pushBackOutput(c);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(byte b)
{
    _pushBackOutput(b);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(short s)
{
    _pushBackBinaryOrder(s);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(uint16 w)
{
    _pushBackBinaryOrder(w);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(int i)
{
    _pushBackBinaryOrder(i);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(uint32 d)
{
    _pushBackBinaryOrder(d);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(long l)
{
    _pushBackBinaryOrder(l);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(unsigned long ul)
{
    _pushBackBinaryOrder(ul);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const int64& l)
{
    _pushBackBinaryOrder(l);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const uint64& q)
{
    _pushBackBinaryOrder(q);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const double& d)
{
    _pushBackBinaryOrder( *reinterpret_cast<const uint64*>(&d) );
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const void* p)
{
    _pushBackBinaryOrder<uint64>( reinterpret_cast<uint64>(p) );
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const char* cs)
{
    size_type i = 0;
    while ( cs[i] )
        _pushBackOutput( cs[i++] );
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(const generic_string& s)
{
    _pushBackOutputString(s);
    if ( !does_buffer_output() )
        _outDevice();
    return *this;
}
rbinstream& rbinstream::operator <<(endianness flag)
{
    _endianFlag = flag;
    return *this;
}
rbinstream& rbinstream::operator <<(binary_string_input_format flag)
{
    _stringInputFormat = flag;
    return *this;
}
template<class Numeric>
void rbinstream::_pushBackBinaryOrder(Numeric n)
{
    // (assume endianness from _endianFlag)
    // divide n into its bytes and put into stream
    ssize_type byteCnt = ssize_type( sizeof(Numeric) );
    if (_endianFlag==big)
    {// get most significant to least significant
        for (ssize_type i = byteCnt-1;i>=0;i--)
            _pushBackOutput( (n>>(8*i))&0xff );
    }
    else if (_endianFlag==little)
    {// get least significant to most significant
        for (ssize_type i = 0;i<byteCnt;i++)
            _pushBackOutput( (n>>(8*i))&0xff );
    }
}
template<class Numeric>
Numeric rbinstream::_fromString(const str& s,bool& success)
{
    // (assume endianness from _endianFlag)
    Numeric val = 0;
    size_type byteCnt = sizeof(Numeric);
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
