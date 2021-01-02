#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class odbcString{
    public:
        odbcString(){
            //std::cout << "default myvalue create\n";
        }
        odbcString(const char * v)
            : string(v)
            , length(strlen(v)){
        }
        const char *string;
        int length;

    };
    using Ty_Result = std::tuple<
        std::string /*error_code*/, 
        std::string /*error_desc*/>;
    enum JackPotIndex{
        CODE = 0,
        ERROR_MESSAGE,
    };

}


