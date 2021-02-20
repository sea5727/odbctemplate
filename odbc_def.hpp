#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    using Ty_Result = std::tuple<
        std::string /*error_code*/, 
        std::string /*error_desc*/>;
    enum JackPotIndex{
        CODE = 0,
        ERROR_MESSAGE,
    };
}


