#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcConnectBuilder;
    class OdbcConnect{
        friend class OdbcConnectBuilder;
        friend class OdbcConnectCtl;
    public:
        std::shared_ptr<OdbcDbc> dbc;
    public:
        explicit OdbcConnect() = default;
    
    public:

        /**
         * @brief BASE STMT 핸들러 
         * SQLAllocHandle 를 호출
         * 
         * @return OdbcResetedStmt 
         */
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


        /**
         * @brief preparedStmt 핸들러를 얻는다.
         * 
         * @param query 
         * @return OdbcpreparedStmt 
         */
        OdbcpreparedStmt
        preparedStmt(const std::string & query){
            return allocStmt().preparedStmt(query);
        }

        /**
         * @brief preparedStmt 핸들러로 직접 쿼리를 한다. ( bind 불가 )
         * 
         * @param query 
         * @return OdbcpreparedStmt 
         */
        OdbcFetcher 
        preparedExecute(const std::string & query){
            return allocStmt().preparedStmt(query).Execute();
        }


        /**
         * @brief prepared 핸들러로 직접 쿼리를 한다. ( bind 가능 )
         * 
         * @tparam Param1 
         * @tparam Params 
         * @param query 
         * @param p1 first bind variable
         * @param rest others bind variables
         * @return OdbcFetcher 
         */
        template <typename Param1, typename... Params>
        OdbcFetcher 
        preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
            return allocStmt().preparedStmt(query).bindExecute(p1, rest...);
        }

        /**
         * @brief 직접 쿼리를 한다. ( bind 불가 )
         * 
         * @param query 
         * @return OdbcFetcher 
         */
        OdbcFetcher 
        directExecute(const std::string & query){
            return allocStmt().directExecute(query);
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