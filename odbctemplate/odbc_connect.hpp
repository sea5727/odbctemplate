#pragma once

#include "odbc_dbc.hpp"
#include "odbc_helper.hpp"
#include "odbc_error.hpp"
#include "odbc_stmt.hpp"
#include "odbc_prepared_stmt.hpp"
#include "odbc_manager.hpp"
#include "odbc_fetcher.hpp"
#include "odbc_reseted_stmt.hpp"


namespace odbctemplate
{
    
    class OdbcConnect{
    private:
        class OdbcConnectProperties{
            friend class OdbcConnect;
            friend class Builder;
        private:
            OdbcConnectProperties() = default;

            std::string dsn     = "";
            long login_timeout  = 0;
            long connect_timeout= 0;
            bool autocommit     = true;
        };

        std::shared_ptr<OdbcDbc> dbc;
        OdbcConnectProperties properties;


    public:
    class OdbcConnectBuilder{
        private:
            std::shared_ptr<OdbcDbc> dbc;
        public:
            OdbcConnectBuilder(const OdbcConnectBuilder & copy)
                : dbc{copy.dbc} {
                std::cout << "copy constructor\n";
            }
            OdbcConnectBuilder(const std::string & dsn)  {
                properties.dsn = dsn;

                odbctemplate::OdbcManager & pm = odbctemplate::OdbcManager::getInstance();
                auto & env = pm.env;
                pm.init_odbc();

                SQLHDBC dbc_;
                
                SQLRETURN status = SQLAllocHandle (SQL_HANDLE_DBC, env, &dbc_);
                if (status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw("get_connection SQLAllocHandle SQL_HANDLE_DBC fail :" + std::to_string(status));
                }
                dbc = std::make_shared<OdbcDbc>(dbc_);
                std::cout << "make dbc: " << dbc_ << ", shared_ptr:" << dbc->dbc << std::endl;
                
            }
            OdbcConnectBuilder(OdbcConnect & conn)
                : dbc{conn.dbc}
                , properties{conn.properties} {

            }

            OdbcConnectBuilder & 
            setLoginTimeout(const long login_timeout){
                properties.login_timeout = login_timeout;
                SQLRETURN status = SQLSetConnectAttr(dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) properties.login_timeout, 0);
                if (status != SQL_SUCCESS){
                    auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc->dbc);
                    SQLFreeConnect(dbc->dbc);
                    dbc->dbc = SQL_NULL_HDBC;
                    odbctemplate::OdbcError::Throw(error);
                }
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
                properties.autocommit = autocommit;
                return *this;
            }
            // SQL_AUTOCOMMIT_OFF 0
            // SQL_AUTOCOMMIT_ON 1
            bool
            getAutocommit() {
                SQLINTEGER value;
                SQLRETURN status = SQLGetConnectAttr(dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
                if (status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
                }
                return (bool)value; 
            }


            [[deprecated("Don't Believe That ! The SQL Server Native Client ODBC driver ignores the setting of SQL_ATTR_CONNECTION_TIMEOUT.")]]
            OdbcConnectBuilder & 
            setConnectTimeout(const long connect_timeout){
                properties.connect_timeout = connect_timeout;
                SQLRETURN status = SQLSetConnectAttr (dbc->dbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER) &properties.connect_timeout, SQL_IS_POINTER);
                if (status != SQL_SUCCESS){
                    auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc->dbc);
                    status = SQLFreeConnect(dbc->dbc);
                    dbc.reset();
                    odbctemplate::OdbcError::Throw(error); 
                }
                return *this;
            }
            [[deprecated("Don't Believe That ! The SQL Server Native Client ODBC driver ignores the setting of SQL_ATTR_CONNECTION_TIMEOUT.")]]
            long
            getConnectTimeout(){
                SQLINTEGER value;
                SQLRETURN status = SQLGetConnectAttr(dbc->dbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
                if (status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
                }
                return value;
            }

            // SQL_CD_TRUE 1 ( Connection is CLOSED )
            // SQL_CD_FALSE 0 ( Connection is OPEN/Available )
            bool
            getConnectionDead() {
                SQLINTEGER 	value;
                SQLRETURN status = SQLGetConnectAttr(dbc->dbc, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER) &value, 0, NULL);
                if (status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
                }
                return (bool)value;
            }

            OdbcConnect
            build(){

                SQLRETURN disstatus = SQLDisconnect(dbc->dbc);

                SQLRETURN status = SQLDriverConnect(dbc->dbc, NULL, (SQLCHAR *)properties.dsn.c_str(), properties.dsn.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
                if (status != SQL_SUCCESS){
                    auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, dbc->dbc);
                    SQLFreeConnect(dbc->dbc);
                    dbc.reset();
                    odbctemplate::OdbcError::Throw(error);
                }
                return OdbcConnect{properties, dbc};
            }
        private:
            OdbcConnectProperties properties;
        };

    public:
        explicit OdbcConnect() = default;
        explicit OdbcConnect(OdbcConnectProperties properties, std::shared_ptr<OdbcDbc> dbc) 
            : properties{properties}
            , dbc{dbc}{
            std::cout << "OdbcConnect create\n";
        }
        explicit OdbcConnect(SQLHDBC dbc) 
            : dbc{std::make_shared<OdbcDbc>(dbc)}{
            std::cout << "OdbcConnect create\n";
        }
        explicit OdbcConnect(std::shared_ptr<OdbcDbc> dbc) 
            : dbc{dbc}{
            std::cout << "OdbcConnect shared_ptr create\n";
        }
        OdbcConnect(const OdbcConnect & copy) 
            : dbc{copy.dbc}{
            std::cout << "OdbcConnect create copy\n";
        }
        OdbcConnect(OdbcConnect && copy) 
            : dbc{copy.dbc}{
            std::cout << "OdbcConnect create move\n";
        }
        ~OdbcConnect(){
            std::cout << "OdbcConnect delete\n";
        }

        OdbcResetedStmt 
        allocStmt() {
            SQLRETURN status = 0;
            SQLHDBC stmt = SQL_NULL_HSTMT;

            status = SQLAllocHandle(SQL_HANDLE_STMT, dbc->dbc, &stmt);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }

            return OdbcResetedStmt{stmt};
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