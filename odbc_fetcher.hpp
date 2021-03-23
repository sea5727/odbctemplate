#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{

    class OdbcFetcher{
    public:
        std::shared_ptr<OdbcStmt> stmt;
    public:

        explicit OdbcFetcher() = default;
        explicit OdbcFetcher(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{ }
        explicit OdbcFetcher(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} { }
        OdbcFetcher(const OdbcFetcher & copy) 
            : stmt{copy.stmt}{ }
        explicit OdbcFetcher(OdbcFetcher && move) 
            : stmt{std::move(move.stmt)}{ }

 
        /**
         * @brief Get the Num Result Cols
         * 
         * @return unsigned short int 
         */
        unsigned short int
        getNumResultCols(){
            SQLSMALLINT col = 0;

            SQLRETURN status = SQLNumResultCols(stmt->stmt, &col);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status); // no dbc error;
            }

            return col;
        }

        /**
         * @brief fetch 데이터를 결과 데이터에 bind해주는 helper용 함수.
         * 사용자는 help 함수에 binding 해주는 함수를 작성해야한다.
         * 
         * @param help 
         * @return OdbcpreparedStmt 
         */
        OdbcFetcher
        bindResultCol(std::function<void(BindColHelper & )> help){
            BindColHelper helper{stmt->stmt};
            help(helper);
            return *this;
        }

        OdbcFetcher
        bindCol(const u_short index, Int64 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_LONG, &buffer->Value, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        OdbcFetcher
        bindCol(const u_short index, NullInt64 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_LONG, &buffer->Value, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        OdbcFetcher
        bindCol(const u_short index, Int32 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_SHORT, &buffer->Value, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        OdbcFetcher
        bindCol(const u_short index, NullInt32 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_SHORT, &buffer->Value, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        template<int N>
        OdbcFetcher
        bindCol(const u_short index, Char<N> *buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, NULL);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        template<int N>
        OdbcFetcher
        bindCol(const u_short index, NullChar<N> * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, &buffer->len);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
      
        OdbcFetcher
        getData(const u_short index, NullFloat64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, 
                                index, 
                                SQL_C_FLOAT, 
                                &buffer->Value, 
                                sizeof(buffer->Value), 
                                (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, Float64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_FLOAT, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, NullInt32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SSHORT, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, Int32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SSHORT, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        
        OdbcFetcher
        getData(const u_short index, NullUInt32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_USHORT, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, UInt32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_USHORT, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, NullUInt64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_ULONG, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, UInt64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_ULONG, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, NullInt64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SLONG, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, Int64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SLONG, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        template<int N>
        OdbcFetcher
        getData(const u_short index, NullChar<N> * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        template<int N>
        OdbcFetcher
        getData(const u_short index, Char<N> * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }


        /**
         * @brief fetch를 1회 호출
         * 
         * @return true 
         * @return false 
         */
        bool
        fetch(){

            SQLRETURN status;
            SQLSMALLINT col;

            status = SQLNumResultCols(stmt->stmt, &col);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status); // no dbc error;
            }
            status = SQLFetch(stmt->stmt);

            if(status != SQL_SUCCESS) {
                if(status == SQL_NULL_DATA){
                    odbctemplate::OdbcError::Throw("SQL_NULL_DATA should bind nullable"); 
                }
                // SQL_NO_DATA / SQL_ERROR etc..
                status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
                return false;
            }

            return true;

        }

        /**
         * @brief 쿼리 결과를 fetch 합니다. ( 주로 select 결과 )
         * 
         * @tparam Return_Ty 쿼리에 대응하는 class 또는 struct 타입이어야 합니다.
         * @param help  Return_Ty를 return 하는 헬퍼 함수여야합니다.
         * @param N vector의 초기 크기. performance param
         * @return std::vector<Return_Ty> 
         */
        template<typename Return_Ty>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper &)> && help, int N) {
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }

            SQLRETURN status;
            auto col = getNumResultCols();

            std::vector<Return_Ty> results;
            if(N > 0)
                results.reserve(N);

            if(col > 0 && help != nullptr){
                FetchHelper helper(stmt->stmt);
                while((status = SQLFetch(stmt->stmt)) == SQL_SUCCESS){
                    helper.Clear();    
                    if(col > 0 && status != SQL_NO_DATA){
                        results.push_back(std::move(help(helper)));
                    }
                }
            }

            status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
            return results;
        }

        template<typename Return_Ty>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper &)> && help) {
            return fetch<Return_Ty>(std::move(help), 0);
        }

        long 
        getInsertRowCount(){
            return getRowCount();
        }
        long
        getUpdateRowCount(){
            return getRowCount();
        }
        long
        getDeleteRowCount(){
            return getRowCount();
        }

        /**
         * @brief update/insert/delete 의 쿼리 결과 count
         * 
         * @return long 
         */
        long
        getRowCount(){ // Insert, Update, Delete
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }
  
            SQLRETURN status;
            
            SQLLEN row;
            status = SQLRowCount(stmt->stmt, &row);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status); // no dbc error;
            }

            status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
            return row;
        }
    };



    
}