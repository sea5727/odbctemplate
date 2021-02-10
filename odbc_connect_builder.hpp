#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{

    class OdbcConnectBuilder{
    private:
        std::shared_ptr<OdbcDbc> dbc;
        std::string dsn     = "";
        long login_timeout  = 0;
        long connect_timeout= 0;
        bool autocommit     = true;
    
        OdbcConnect conn;
    public:
        explicit OdbcConnectBuilder() {
            auto & om = odbctemplate::OdbcManager::getInstance();
            om.init_odbc();

            SQLHDBC tmp_dbc;
            SQLRETURN status = SQLAllocHandle (SQL_HANDLE_DBC, om.env, &tmp_dbc);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw("get_connection SQLAllocHandle SQL_HANDLE_DBC fail :" + std::to_string(status));
            }
            dbc = std::make_shared<OdbcDbc>(tmp_dbc);
            conn.dbc = dbc;
        }

        explicit OdbcConnectBuilder(OdbcConnect conn) 
            : conn(conn){

        }


        OdbcConnectBuilder &
        setDsn(const std::string & dsn) {
            //todo validation?
            this->dsn.assign(dsn);
            return *this;
        }


        OdbcConnectBuilder &
        setLoginTimeout(const long login_timeout){
            SQLRETURN status = SQLSetConnectAttr(dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) login_timeout, 0);
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc->dbc);
                SQLFreeConnect(dbc->dbc);
                dbc->dbc = SQL_NULL_HDBC;
                odbctemplate::OdbcError::Throw(error);
            }

            this->login_timeout = login_timeout;

            return *this;
        }

        long
        getLoginTimeout(){
            SQLINTEGER value;
            SQLRETURN status = SQLGetConnectAttr(dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }
            return value;
        }


        OdbcConnectBuilder & 
        setAutocommit(bool autocommit){

            SQLRETURN status = SQLSetConnectAttr (dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)autocommit, 0); // if set to a pointer, a failure occurs
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc->dbc);
                SQLFreeConnect(dbc->dbc);
                dbc.reset();
                odbctemplate::OdbcError::Throw(error);
            }
            this->autocommit = autocommit;
            return *this;
        }

        /**
         * @brief Get the Autocommit object
         * 
         * @return true SQL_AUTOCOMMIT_ON 1
         * @return false SQL_AUTOCOMMIT_OFF 0
         */
        bool
        getAutocommit() {
            SQLINTEGER value;
            SQLRETURN status = SQLGetConnectAttr(dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }
            return (bool)value; 
        }

        bool
        getConnectionDead() {
            SQLINTEGER 	value;
            SQLRETURN status = SQLGetConnectAttr(dbc->dbc, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER) &value, 0, NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }
            return (bool)value;
        }

        OdbcConnect &&
        build(){
            SQLRETURN disstatus = SQLDisconnect(dbc->dbc);

            SQLRETURN status = SQLDriverConnect(dbc->dbc, NULL, (SQLCHAR *)dsn.c_str(), dsn.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc->dbc);
                SQLFreeConnect(dbc->dbc);
                dbc.reset();
                odbctemplate::OdbcError::Throw(error);
            }
            return std::move(conn);
        }



    };

    OdbcConnectBuilder 
    OdbcConnect::builder(){
        return OdbcConnectBuilder ();
    }

}