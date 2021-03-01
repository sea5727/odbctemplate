#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class Nullable {
    public:
        Nullable() {
            len = -1;
        }
        long len;

        inline
        bool
        Valid(){
            return len != -1;
        }
    };

    class NullString : public Nullable {
    public:
        std::string String;
    };
    template<int N>
    class NullChar : public Nullable {
    public:
        char Char[N];
    };
    template<>
    class NullChar<1> : public Nullable {
    public:
        char Char;
    };
    class NullInt64 : public Nullable {
    public:
        int64_t Int64;
    };
    class NullInt32 : public Nullable {
    public:
        int32_t Int32;
    };
    class NullBool : public Nullable {
    public:
        bool Bool;
    };
    class NullFloat64 : public Nullable {
    public:
        float Float64;  
    };

    template<int N>
    class Char {
    public:
        char Char[N];
    };
    template<>
    class Char<1> {
    public:
        char Char;
    };
    class String {
    public:
        std::string String;    
    };
    class Int64 {
    public:
        int64_t Int64;
    };
    class Int32 {
    public:
        int32_t Int32;
    };
    class Bool {
    public:
        bool Bool;
    };
    class Float64 {
    public:
        float Float64;
    };

}