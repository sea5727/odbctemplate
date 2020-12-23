#pragma once

#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <sql.h>
#include <sqlext.h>

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