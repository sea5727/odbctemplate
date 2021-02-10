#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    
    class OdbcResetedStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcResetedStmt() = default;
        explicit OdbcResetedStmt(SQLHSTMT stmt) 
            : stmt(std::make_shared<OdbcStmt>(stmt)) { }
 
    public:
        OdbcpreparedStmt
        preparedStmt(const std::string & query){
            SQLRETURN status = SQLPrepare( stmt->stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcpreparedStmt(stmt);
        }
        
        OdbcFetcher
        directExecute(const std::string & query){
            SQLRETURN status = SQLExecDirect(stmt->stmt, (SQLCHAR *)query.c_str(), query.length());
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher(stmt);
        }


        OdbcFetcher 
        preparedExecute(const std::string & query){
            return preparedStmt(query).Execute();
        }


        template <typename Param1, typename... Params>
        OdbcFetcher 
        preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
            return preparedStmt(query).bindExecute(p1, rest...);
        }

    };
}