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
        bindExecute(const Param1 p1, const Params... rest){
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
        class templateString{
        public:
            templateString(){
            }
            templateString(const char * v)
                : string(v)
                , length(strlen(v)){
            }
            const char *string;
            int length;

        };


        void bind(const int index, const templateString & param) {
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