#pragma once

#include <string.h>

#include "odbc_helper.hpp"
#include "odbc_error.hpp"
#include "odbc_fetcher.hpp"
#include "odbc_stmt.hpp"
#include "odbc_prepared_stmt.hpp"


namespace odbctemplate
{
    class OdbcResetedStmt : OdbcStmt{
    private:
        // SQLHSTMT stmt = SQL_NULL_HSTMT;
    public:
        OdbcResetedStmt() = default;
        OdbcResetedStmt(SQLHSTMT stmt) 
            : OdbcStmt{stmt}{
            std::cout << "OdbcResetedStmt create..\n";
        }
        OdbcResetedStmt(const OdbcResetedStmt & copy) 
            : OdbcStmt(0){
            std::cout << "OdbcResetedStmt copy create..\n";
        }
        OdbcResetedStmt(OdbcResetedStmt && move) 
            : OdbcStmt(0){
            std::cout << "OdbcResetedStmt move create..\n";
        }
        ~OdbcResetedStmt(){
            std::cout << "OdbcResetedStmt delete..\n";
            // SQLFreeStmt(stmt, SQL_DROP);
        }
 
    public:
        OdbcPreparedStmt
        prepareStmt(const std::string & query){
            std::cout << "======== " << __func__ << " start ============" << std::endl;

            SQLRETURN status;
            status = SQLPrepare( stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }
            std::cout << "======== " << __func__ << " end ============" << std::endl;
            OdbcStmt * test = this;
            auto result = OdbcPreparedStmt{test};

            return result;
        }


    };
}