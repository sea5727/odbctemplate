#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{

    class OdbcpreparedStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcpreparedStmt() = default;
        explicit OdbcpreparedStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{
            // std::cout << "OdbcpreparedStmt default create..\n";
        }
        explicit OdbcpreparedStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} {
            // std::cout << "OdbcpreparedStmt shared create..\n";
        }
        OdbcpreparedStmt(const OdbcpreparedStmt & copy) 
            : stmt{copy.stmt}{
            // std::cout << "OdbcpreparedStmt copy create..\n";
        }
        explicit OdbcpreparedStmt(OdbcpreparedStmt && move) 
            : stmt{std::move(move.stmt)}{
            // std::cout << "OdbcpreparedStmt move create..\n";
        }
        ~OdbcpreparedStmt(){
            // std::cout << "OdbcpreparedStmt delete..\n";
        }
 
    public:


        
        OdbcpreparedStmt
        bindCol(std::function<void(BindColHelper & )> help){
            BindColHelper helper{stmt->stmt};
            help(helper);
            return *this;
        }

        template <typename Param1, typename... Params>
        OdbcFetcher 
        bindExecute(const Param1 & p1, const Params&... rest){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            bindForParams(1, p1, rest...);
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher{stmt};
        }

        OdbcFetcher 
        Execute(){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher{stmt};
        }

    
    private:
        void bind(const int index, const odbcString & param) {
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
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
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
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        template < typename Params>
        void bindForParams(int index, const Params& param){
            bind(index, param);
        }
        template <typename Param1, typename... Params>
        void bindForParams(int index, const Param1 & param, const Params&... rest){
            bind(index, param);
            bindForParams(++index, rest...);
            return;
        }

        

    };
}