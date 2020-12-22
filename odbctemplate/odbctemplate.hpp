#pragma once

#include <string>
#include <iostream>
#include <tuple>
#include <sql.h>
#include <sqlext.h>
#include <functional>
#include <iostream> 
#include <typeinfo> 


#include "odbc_error.hpp"
#include "odbc_manager.hpp"

#include "odbc_connect.hpp"
#include "odbc_connection_pool.hpp"
#include "odbc_def.hpp"
#include "odbc_helper.hpp"
#include "odbc_stmt.hpp"
#include "odbc_reseted_stmt.hpp"
#include "odbc_prepared_stmt.hpp"
#include "odbc_fetcher.hpp"


namespace odbctemplate
{
    bool debug_print = false;
}