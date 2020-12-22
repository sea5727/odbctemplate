#pragma once

#include "odbc_helper.hpp"
#include "odbc_error.hpp"
#include "odbc_stmt.hpp"
#include "odbc_prepared_stmt.hpp"
#include "odbc_manager.hpp"
#include "odbc_fetcher.hpp"
#include "odbc_reseted_stmt.hpp"


namespace odbctemplate
{
    // OdbcConnect -> allocStmt -> OdbcStmt -> OdbcPreParedStmt -> Binder -> Fetcher
    // OdbcConnect -> preparedStmt(query) -> OdbcPreparedStmt -> Binder -> Fetch
    // OdbcConnect -> queryForObject(query) -> Fetcher
    class OdbcConnect{
    private:
        SQLHDBC dbc = SQL_NULL_HDBC;
    public:
        OdbcConnect() = default;
        OdbcConnect(SQLHDBC dbc) 
            : dbc(dbc){
            //std::cout << "OdbcConnect create\n";
        }
        ~OdbcConnect(){
            // if(dbc != SQL_NULL_HDBC)
            //     SQLFreeConnect(dbc);
        }
        OdbcConnect(OdbcConnect & copy) 
            : dbc(copy.dbc){
            //std::cout << "OdbcConnect create copy\n";
            copy.dbc = SQL_NULL_HDBC;
        }
        OdbcConnect(OdbcConnect && copy) 
            : dbc(copy.dbc){
            //std::cout << "OdbcConnect create move\n";
            copy.dbc = SQL_NULL_HDBC;
        }

        OdbcResetedStmt 
        allocStmt() {
            //std::cout << "======== allocStmt start ============\n";
            SQLRETURN status = 0;

            SQLHDBC stmt = SQL_NULL_HSTMT;

            status = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc);
            }
            //std::cout << "created stmt : " << stmt << std::endl;

            return OdbcResetedStmt{stmt};
        }

        OdbcPreparedStmt
        preparedStmt(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            return allocStmt().prepareStmt(query);
        }
    
        OdbcFetcher 
        directExecute(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            return allocStmt().directExecute(query);
        }

        OdbcFetcher 
        preparedExecute(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;           
            return preparedStmt(query).Execute();
        }


        template <typename Param1, typename... Params>
        OdbcFetcher 
        preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;           
            return preparedStmt(query).bindExecute(p1, rest...);
        }




        // template <typename Param1, typename... Params>
        // Fetcher 
        // preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
        //     //std::cout << "preparedExecute start\n";
        //     auto stmt = allocStmt();
            
        //     return preparedStmt(query).bindExecute(p1, rest...);
        // }


        void
        commit(){
            SQLRETURN status = 0;
            status = SQLTransact(SQL_NULL_HENV, dbc, SQL_COMMIT);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc);
            }
        }
        void
        rollback(){
            SQLRETURN status = 0;
            status = SQLTransact(SQL_NULL_HENV, dbc, SQL_ROLLBACK);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc);
            }
        }

        static
        OdbcConnect
        get_connection( 
            const std::string & conn_info,
            const long auto_commit = 1,
            const long connection_timeout = 10,
            const long login_timeout = 10
            ){
            
            //std::cout << "get_connection.." << conn_info << auto_commit << connection_timeout << login_timeout << std::endl;
            odbctemplate::OdbcManager & pm= odbctemplate::OdbcManager::getInstance();

            auto & env = pm.env;

            SQLHDBC dbc;

            SQLRETURN status = 0;

            pm.init_odbc();
            // get handle
            status = SQLAllocHandle (SQL_HANDLE_DBC, env, &dbc);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw("get_connection SQLAllocHandle SQL_HANDLE_DBC fail");
            }
            status = SQLSetConnectAttr (dbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER) &connection_timeout, SQL_IS_POINTER);
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc);
                SQLFreeConnect(dbc);
                dbc = SQL_NULL_HDBC;
                odbctemplate::OdbcError::Throw(error);
            }
            status = SQLSetConnectAttr(dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) &login_timeout, SQL_IS_POINTER);
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc);
                SQLFreeConnect(dbc);
                dbc = SQL_NULL_HDBC;
                odbctemplate::OdbcError::Throw(error);
            }
            status = SQLSetConnectAttr (dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)auto_commit, 0); // if set to a pointer, a failure occurs
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc);
                SQLFreeConnect(dbc);
                dbc = SQL_NULL_HDBC;
                odbctemplate::OdbcError::Throw(error);
            }
            // // const char *conn_info = "DSN=TST_DB;";
            status = SQLDriverConnect(dbc, NULL, (SQLCHAR *)conn_info.c_str(), conn_info.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc);
                SQLFreeConnect(dbc);
                dbc = SQL_NULL_HDBC;
                odbctemplate::OdbcError::Throw(error);
            }
            return {dbc};
        }

        

    };
}