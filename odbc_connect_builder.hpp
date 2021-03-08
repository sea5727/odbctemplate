#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcConnectBuilder{
    private:
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
            auto dbc = std::make_shared<OdbcDbc>(tmp_dbc);
            conn.dbc = dbc;
        }

        explicit OdbcConnectBuilder(OdbcConnect conn) 
            : conn(conn){

        }


        OdbcConnectBuilder &
        setDsn(const std::string & dsn) {
            //todo validation?
            OdbcConnectCtl::connectDB(conn, dsn);
            return *this;
        }


        OdbcConnectBuilder &
        setLoginTimeout(const long login_timeout){
            OdbcConnectCtl::setLoginTimeout(conn, login_timeout);
            return *this;
        }

        OdbcConnectBuilder & 
        setAutocommit(bool autocommit){
            OdbcConnectCtl::setAutocommit(conn, autocommit);
            return *this;
        }

        OdbcConnect &&
        build(){
            return std::move(conn);
        }



    };
}

