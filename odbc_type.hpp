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
        std::string Value;
    };
    template<int N>
    class NullChar : public Nullable {
    public:
        char Value[N];
    };
    template<>
    class NullChar<1> : public Nullable {
    public:
        char Value;
    };
    class NullInt64 : public Nullable {
    public:
        int64_t Value;
    };
    class NullUInt64 : public Nullable {
    public:
        uint64_t Value;
    };
    class NullInt32 : public Nullable {
    public:
        int32_t Value;
    };
    class NullUInt32 : public Nullable {
    public:
        uint64_t Value;
    };
    class NullBool : public Nullable {
    public:
        bool Value;
    };
    class NullFloat64 : public Nullable {
    public:
        float Value;  
    };

    template<int N>
    class Char {
    public:
        char Value[N];
    };
    template<>
    class Char<1> {
    public:
        char Value;
    };
    class String {
    public:
        std::string Value;    
    };
    class Int64 {
    public:
        int64_t Value;
    };
    class UInt64 {
    public:
        uint64_t Value;
    };
    class Int32 {
    public:
        int32_t Value;
    };
    class UInt32 {
    public:
        uint32_t Value;
    };
    class Bool {
    public:
        bool Value;
    };
    class Float64 {
    public:
        float Value;
    };

}