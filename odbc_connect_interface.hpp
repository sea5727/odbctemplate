#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcConnectCtl {
    public:
        static
        void 
        setLoginTimeout(OdbcConnect & conn, const long login_timeout){
            SQLRETURN status = SQLSetConnectAttr(conn.dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) login_timeout, 0);
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                odbctemplate::OdbcError::Throw(error);
            }
        }
        static
        void
        setAutocommit(OdbcConnect & conn, bool autocommit) {
            SQLRETURN status = SQLSetConnectAttr (conn.dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)autocommit, 0); // if set to a pointer, a failure occurs
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                odbctemplate::OdbcError::Throw(error);
            }
        }
        static
        void
        connectDB(OdbcConnect & conn, const std::string & dsn){
            static std::mutex mtx;
            mtx.lock();
            SQLRETURN status = SQLDriverConnect(conn.dbc->dbc, NULL, (SQLCHAR *)dsn.c_str(), dsn.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                mtx.unlock();
                odbctemplate::OdbcError::Throw(error);
                // auto code = std::get<0>(error);
                // if(code.compare("08002") != 0){
                //     SQLDisconnect(conn.dbc->dbc);
                //     status = SQLDriverConnect(conn.dbc->dbc, NULL, (SQLCHAR *)dsn.c_str(), dsn.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
                //     if (status != SQL_SUCCESS){
                //         mtx.unlock();
                //         odbctemplate::OdbcError::Throw(error);
                //     }
                // }
            }
            mtx.unlock();
        }

        /**
         * @brief Get the Autocommit object
         * 
         * @return true SQL_AUTOCOMMIT_ON 1
         * @return false SQL_AUTOCOMMIT_OFF 0
         */
        static
        bool
        getAutocommit(OdbcConnect & conn){
            SQLINTEGER value;
            SQLRETURN status = SQLGetConnectAttr(conn.dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return (bool)value; 
        }

        static
        bool
        getConnectionDead(OdbcConnect & conn) {
            SQLINTEGER 	value = true;
            SQLRETURN status = SQLGetConnectAttr(conn.dbc->dbc, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER) &value, 0, NULL);
            printf("getConnectionDead:%d, %d\n", status, value);
            if (status != SQL_SUCCESS){
                SQLDisconnect(conn.dbc->dbc);
                // odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return (bool)value;
        }


        static
        void
        disconnectDB(OdbcConnect & conn) {
            SQLRETURN status = SQLDisconnect(conn.dbc->dbc);
            if (status != SQL_SUCCESS){
                // odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return ;
        }


        static
        long
        getLoginTimeout(OdbcConnect & conn){
            SQLINTEGER value;
            SQLRETURN status = SQLGetConnectAttr(conn.dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return value;
        }

    };

}