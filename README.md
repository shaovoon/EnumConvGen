# EnumConvGen
C++ Enum to String Converter Generator

*Introduction*

EnumConvGen is a Enum to String Converter Generator written in C++ transpiled to asm.js using Emscripten.

Enum format

EnumConvGen accept C++11 scoped enum and C++98 enum. Scoped enum for C++98 enum can be emulated by declaring it inside structure or namespace.

```// C++11 scoped enum
enum class Gender
{
    Male='M', Female='F'
};

// C++98 enum
enum Place
{
    Forest, Mountain
};

// C++98 enum inside struct
struct Color
{
    enum Value
    {
        Red='r', Green=0xFF00, Blue
    };
};

// C++98 enum inside namespace
namespace Number
{
    enum Value
    {
        One=1, Two, Three
    };
};```

Enum type can be specified.

```// C++11 scoped enum
enum class Gender : uint8
{
    Male='M', Female='F'
};```

For C++98 enum, type can be indicated to parser by putting the type in comments

```// C++98 enum
enum Gender /* : uint8 */
{
    Male='M', Female='F'
};```

Type must be single word without whitespace!

// This is bad!
enum class Gender : unsigned char
{
    Male='M', Female='F'
};
Enum value must not be an expression!

```red='r', // ok
red=0xff0000, // ok
red=1, // ok
red=std::max(sizeof(type), sizeof(long)), // cannot be parsed```


*Generate code*

Let's fire up the EnumConvGen.html to generate the converters based on the enum below.

```namespace Number
{
    enum Value
    {
        One=1, Two, Three
    };
};```

6 functions are generated.

```std::string to_string(Number::Value const & val);
void from_string(std::string const & val, /* OUT */ Number::Value & out_val);
int to_int(Number::Value const & val);
void from_int(int const & val, /* OUT */ Number::Value & out_val);
std::ostream & operator<<(std::ostream & os, Number::Value const & val);
std::istream & operator>>(std::istream & is, Number::Value & val);

std::string to_string(Number::Value const & val)
{
    switch(val)
    {
        case (int)Number::One: 
            return "One";
        case (int)Number::Two: 
            return "Two";
        case (int)Number::Three: 
            return "Three";
    }
    return "";
}

void from_string(std::string const & val, /* OUT */ Number::Value & out_val)
{
    if( val == "One" )
    { out_val = Number::One; }
    else if( val == "Two" )
    { out_val = Number::Two; }
    else if( val == "Three" )
    { out_val = Number::Three; }
    else
    {
        std::ostringstream oss;
        oss << "Number::Value unknown match in string:" << val;
        throw std::runtime_error(oss.str());
    }
}

int to_int(Number::Value const & val)
{
    return (int)val;
}

void from_int(int const & val, /* OUT */ Number::Value & out_val)
{
    switch(val)
    {
        case (int)Number::One: 
            out_val = Number::One; break;
        case (int)Number::Two: 
            out_val = Number::Two; break;
        case (int)Number::Three: 
            out_val = Number::Three; break;
        default:
        {
            std::ostringstream oss;
            oss << "Number::Value unknown match in integral:" << val;
            throw std::runtime_error(oss.str());
        }
    }
}

std::ostream & operator<<(std::ostream & os, Number::Value const & val)
{
    return os << to_string(val);
}

std::istream & operator>>(std::istream & is, Number::Value & val)
{
    std::string str;
    is >> str;
    from_string(str, val);
    return is;
}```

*Note*

EnumConvGen is a dumb parser: it cannot recognise comments or expression so put strictly enum code. It also does not check for syntax errors. However it can parse more than 1 enum at the same time. Don't like the generated code? Then fork it on Github and replace the output function with yours. Emscripten Makefile is provided, invoke it by specifying the file.

```make -f MakefileEmscripten```