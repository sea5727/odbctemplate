#pragma once

#include "odbc_helper.hpp"
#include "odbc_error.hpp"

namespace odbctemplate
{
    class OdbcManager{
    private:
        OdbcManager() = default;
        ~OdbcManager(){
            SQLFreeEnv(env);
        }

    public:
        SQLHENV env = SQL_NULL_HENV;
        static OdbcManager & getInstance(){
            static OdbcManager * instance = nullptr;
            if(instance == nullptr){
                instance = new OdbcManager;
            }
            return *instance;
        }

        OdbcManager &
        init_odbc(){

            odbctemplate::OdbcManager & pm = odbctemplate::OdbcManager::getInstance();

            if(env == SQL_NULL_HENV){
                SQLRETURN status = 0;
                status = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
                if(status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw("init_odbc SQLAllocHandle SQL_HANDLE_ENV fail");
                }
                status = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
                if(status != SQL_SUCCESS){
                    auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_ENV, env);
                    SQLFreeEnv(env);
                    env = SQL_NULL_HENV;
                    odbctemplate::OdbcError::Throw(error);
                }
            }
            return pm;
        }




    };

}