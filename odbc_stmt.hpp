#pragma once

#include <string.h>

#include "odbc_helper.hpp"
#include "odbc_error.hpp"
#include "odbc_fetcher.hpp"



namespace odbctemplate
{
    class Fetcher;
    class OdbcStmt{
    private:
        SQLHSTMT stmt = SQL_NULL_HSTMT;
        // char buffer[10][256] = {"", };

    public:
        OdbcStmt() = default;
        OdbcStmt(SQLHSTMT stmt) 
            : stmt(stmt){
        }
        ~OdbcStmt(){
            SQLFreeStmt(stmt, SQL_DROP);
        }
 
    public:
        Fetcher
        queryForObject(const std::string & query){

            SQLRETURN status = 0;

            status = SQLExecDirect(stmt, (SQLCHAR *)query.c_str(), query.length());
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }

            return {stmt};
        }
        template <typename Param1, typename... Params>
        Fetcher 
        queryForObject(const std::string & query, const Param1 & p1, const Params&... rest){
            
            SQLRETURN status = 0;
            status = SQLPrepare( stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }

            bindForParams(1, p1, rest...);
            status = SQLExecute(stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }
            return {stmt};
        }

        template <typename Param1, typename... Params>
        Fetcher 
        requeryForObject(const Param1 &p1, const Params&... rest){
            SQLRETURN status = 0;

            bindForParams(1, p1, rest...);
            
            status = SQLExecute(stmt);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }
            
            return {stmt};
        }


        
    private:
        void bind(const int index, const odbcString & param) {
            std::cout << "new bind.. index : " << index << ", param :" << param.string << ", leng:" << param.length << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)param.string,
                                    param.length,
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }
        }
        void bind(const int index, const int & param) {
            SQLRETURN status = 1;

            status = SQLBindParameter( stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_SLONG,
                                    SQL_INTEGER,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
            }
        }

        template < typename Params>
        void bindForParams(int index, const Params& param){
            bind(index, param);
        }
        template <typename Param1, typename... Params>
        void bindForParams(int index, const Param1 & param, const Params&... rest){
            std::cout << "bindForParams start\n";
            bind(index, param);
            bindForParams(++index, rest...);
            return;
        }




    };
}