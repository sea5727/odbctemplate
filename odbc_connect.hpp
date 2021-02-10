#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcConnectBuilder;
    class OdbcConnect{
        friend class OdbcConnectBuilder;
    private:
        std::shared_ptr<OdbcDbc> dbc;
    public:
        explicit OdbcConnect() = default;
        static OdbcConnectBuilder builder();
        OdbcResetedStmt 
        allocStmt() {
            SQLRETURN status = 0;
            SQLHDBC stmt = SQL_NULL_HSTMT;

            status = SQLAllocHandle(SQL_HANDLE_STMT, dbc->dbc, &stmt);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }

            return OdbcResetedStmt(stmt);
        }

        OdbcpreparedStmt
        preparedStmt(const std::string & query){
            return allocStmt().preparedStmt(query);
        }
        
    
        OdbcFetcher 
        directExecute(const std::string & query){
            return allocStmt().directExecute(query);
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

        void
        commit(){
            SQLRETURN status = SQLTransact(SQL_NULL_HENV, dbc->dbc, SQL_COMMIT);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }
        void
        rollback(){
            SQLRETURN status = SQLTransact(SQL_NULL_HENV, dbc->dbc, SQL_ROLLBACK);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }
    };
}