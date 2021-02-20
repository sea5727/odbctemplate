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
        connectDb(OdbcConnect & conn, const std::string & dsn){
            SQLRETURN status = SQLDriverConnect(conn.dbc->dbc, NULL, (SQLCHAR *)dsn.c_str(), dsn.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                odbctemplate::OdbcError::Throw(error);
            }
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
            SQLINTEGER 	value;
            SQLRETURN status = SQLGetConnectAttr(conn.dbc->dbc, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER) &value, 0, NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return (bool)value;
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