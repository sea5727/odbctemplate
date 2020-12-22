#pragma once

#include <string.h>

#include "odbc_helper.hpp"
#include "odbc_error.hpp"
#include "odbc_fetcher.hpp"
#include "odbc_stmt.hpp"
#include "odbc_prepared_stmt.hpp"
#include "odbc_fetcher.hpp"


namespace odbctemplate
{
    
    class OdbcResetedStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcResetedStmt() = default;
        explicit OdbcResetedStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)} {
            //std::cout << "OdbcResetedStmt create..\n";
        }
        explicit OdbcResetedStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} {
            //std::cout << "OdbcResetedStmt create..\n";
        }
        explicit OdbcResetedStmt(const OdbcResetedStmt & copy)
            : stmt{copy.stmt} {
            //std::cout << "OdbcResetedStmt copy create..\n";
        }
        explicit OdbcResetedStmt(OdbcResetedStmt && move) 
            : stmt{std::move(move.stmt)} {
            //std::cout << "OdbcResetedStmt move create..\n";
        }
        ~OdbcResetedStmt(){
            //std::cout << "OdbcResetedStmt delete..\n";
        }
 
    public:
        OdbcPreparedStmt
        prepareStmt(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            SQLRETURN status = SQLPrepare( stmt->stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt);
            }
            //std::cout << "======== " << __func__ << " end ============" << std::endl;
            return OdbcPreparedStmt{std::move(stmt)};
        }
        
        OdbcFetcher
        directExecute(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            SQLRETURN status = SQLExecDirect(stmt->stmt, (SQLCHAR *)query.c_str(), query.length());
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt);
            }
            return OdbcFetcher{std::move(stmt)};
        }
        void
        print_mystmt(){
            //std::cout << "print_mystmt OdbcPreparedStmt, " << stmt->stmt << std::endl;
        }
    };
}