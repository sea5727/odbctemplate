#pragma once

#include <string.h>

#include "odbc_helper.hpp"
#include "odbc_error.hpp"
#include "odbc_fetcher.hpp"
#include "odbc_stmt.hpp"


namespace odbctemplate
{
    class OdbcPreparedStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcPreparedStmt() = default;
        explicit OdbcPreparedStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{
            //std::cout << "OdbcPreparedStmt create..\n";
        }
        explicit OdbcPreparedStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} {
            //std::cout << "OdbcPreparedStmt create..\n";
        }
        explicit OdbcPreparedStmt(const OdbcPreparedStmt & copy) 
            : stmt{copy.stmt}{
            //std::cout << "OdbcPreparedStmt copy create..\n";
        }
        explicit OdbcPreparedStmt(OdbcPreparedStmt && move) 
            : stmt{move.stmt}{
            //std::cout << "OdbcPreparedStmt move create..\n";
        }
        ~OdbcPreparedStmt(){
            //std::cout << "OdbcPreparedStmt delete..\n";
            // SQLFreeStmt(stmt, SQL_DROP);
        }
 
    public:
        void
        print_mystmt(){
            //std::cout << "print_mystmt OdbcPreparedStmt, " << stmt->stmt << std::endl;
        }

        template <typename Param1, typename... Params>
        OdbcFetcher 
        bindExecute(const Param1 & p1, const Params&... rest){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            bindForParams(1, p1, rest...);
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt);
            }
            return OdbcFetcher{std::move(stmt)};
        }

        OdbcFetcher 
        Execute(){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt);
            }
            return OdbcFetcher{std::move(stmt)};
        }

    private:
        void bind(const int index, const odbcString & param) {
            //std::cout << "bind ! index :" << index << ", param : " << param.string << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
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
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt);
            }
        }
        void bind(const int index, const int & param) {
            //std::cout << "bind ! index :" << index << ", param : " << param << std::endl;
            SQLRETURN status = 1;

            status = SQLBindParameter( stmt->stmt,
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
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt);
            }
        }

        template < typename Params>
        void bindForParams(int index, const Params& param){
            bind(index, param);
        }
        template <typename Param1, typename... Params>
        void bindForParams(int index, const Param1 & param, const Params&... rest){
            //std::cout << "bind start\n";
            bind(index, param);
            //std::cout << "bindForParams start\n";
            bindForParams(++index, rest...);
            //std::cout << "bindForParams  end.. start\n";
            return;
        }

        

    };
}