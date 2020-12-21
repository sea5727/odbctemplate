#pragma once

#include <string.h>

#include "odbc_helper.hpp"
#include "odbc_error.hpp"
#include "odbc_fetcher.hpp"
#include "odbc_stmt.hpp"



namespace odbctemplate
{
    class OdbcPreparedStmt : OdbcStmt{
    private:
        // SQLHSTMT stmt = SQL_NULL_HSTMT;
    public:
        OdbcPreparedStmt() = default;
        OdbcPreparedStmt(SQLHSTMT stmt) 
            : OdbcStmt{stmt}{
            std::cout << "OdbcPreparedStmt create..\n";
        }
        OdbcPreparedStmt(const OdbcPreparedStmt & copy) 
            : OdbcStmt(0){
            std::cout << "OdbcPreparedStmt copy create..\n";
        }
        OdbcPreparedStmt(OdbcPreparedStmt && move) 
            : OdbcStmt(0){
            std::cout << "OdbcPreparedStmt move create..\n";
        }
        ~OdbcPreparedStmt(){
            std::cout << "OdbcPreparedStmt delete..\n";
            // SQLFreeStmt(stmt, SQL_DROP);
        }
 
    public:

        template <typename Param1, typename... Params>
        Fetcher 
        bindExecute(const Param1 & p1, const Params&... rest){

            std::cout << "bindExecute start\n";
            SQLRETURN status = 0;

            std::cout << "bindForParams start\n";
            bindForParams(1, p1, rest...);
            std::cout << "SQLExecute start\n";
            status = SQLExecute(stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }
            return {stmt};
        }

    };
}