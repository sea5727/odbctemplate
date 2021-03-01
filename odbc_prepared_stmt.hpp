#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcpreparedStmt{
        friend class OdbcpreparedUtil;
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcpreparedStmt() = default;
        explicit OdbcpreparedStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{ }
        explicit OdbcpreparedStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} { }
        ~OdbcpreparedStmt(){ }
 
    public:
        /**
         * @brief fetch 데이터를 결과 데이터에 bind해주는 helper용 함수.
         * 사용자는 help 함수에 binding 해주는 함수를 작성해야한다.
         * 
         * @param help 
         * @return OdbcpreparedStmt 
         */
        OdbcpreparedStmt
        bindResultCol(std::function<void(BindColHelper & )> help){
            BindColHelper helper{stmt->stmt};
            help(helper);
            return *this;
        }

        template <typename Param1, typename... Params>
        OdbcFetcher 
        bindExecute(const Param1 & p1, const Params&... rest){
            // std::cout << "p1 : " << (void *)&p1 << ", type:" << typeid(p1).name() << std::endl;
            bindForParams(1, p1, rest...);
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher{stmt};
        }

        OdbcFetcher 
        Execute(){
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher{stmt};
        }
    
    private:
        template< unsigned N >
        void bind(const int index, const char (&arr)[N] ){
            // std::cout << "bind const char (&arr)[N].." << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)arr,
                                    strlen(arr),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }


        void bind(const int index, const std::string & param) {
            // std::cout << "bind const std::string.." << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)param.c_str(),
                                    param.length(),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void bind(const int index, char * const &arr ){
            // std::cout << "bind const char *arr.." << (void *)&arr << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)arr,
                                    strlen(arr),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }        

        void bind(const int index, const uint & param) {
            std::cout << "bind const uint.." << std::endl;
            SQLRETURN status = 1;

            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_ULONG,
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

        void bind(const int index, const int & param) {
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
            // // std::cout << "bindForParams p1 : " << (void *)&param << ", type:" << typeid(param).name() << std::endl;
            bind(index, param);
        }
        template <typename Param1, typename... Params>
        void bindForParams(int index, const Param1 & param, const Params&... rest){
            // // std::cout << "bindForParams p1 : " << (void *)&param << ", type:" << typeid(param).name() << std::endl;
            bind(index, param);
            bindForParams(++index, rest...);
            return;
        }
    };


}