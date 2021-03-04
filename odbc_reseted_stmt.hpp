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
        /**
         * @brief preparedStmt 핸들러를 얻는다.
         * 
         * @param query 
         * @return OdbcpreparedStmt 
         */
        OdbcpreparedStmt
        preparedStmt(const std::string & query){
            SQLRETURN status = SQLPrepare( stmt->stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcpreparedStmt(stmt);
        }
        
        /**
         * @brief 직접 쿼리를 한다. ( bind 불가 )
         * 
         * @param query 
         * @return OdbcFetcher 
         */
        OdbcFetcher
        directExecute(const std::string & query){
            SQLRETURN status = SQLExecDirect(stmt->stmt, (SQLCHAR *)query.c_str(), SQL_NTS);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher(stmt);
        }

        /**
         * @brief preparedStmt 핸들러로 쿼리를 한다 ( bind 불가 )
         * 
         * @param query 
         * @return OdbcFetcher 
         */
        OdbcFetcher 
        preparedExecute(const std::string & query){
            return preparedStmt(query).Execute();
        }

        /**
         * @brief preparedStmt 핸들러로 쿼리를 한다 ( bind 가능 )
         * 
         * @tparam Param1 
         * @tparam Params 
         * @param query 
         * @param p1 
         * @param rest 
         * @return OdbcFetcher 
         */
        template <typename Param1, typename... Params>
        OdbcFetcher 
        preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
            return preparedStmt(query).bindExecute(p1, rest...);
        }

    };
}