#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <sql.h>
#include <sqlext.h>

#include "odbc_stmt.hpp"

namespace odbctemplate
{
    class Fetcher : OdbcStmt{
    private:
        // SQLHSTMT stmt = SQL_NULL_HSTMT;
    public:

        Fetcher() = default;
        Fetcher( SQLHSTMT stmt) 
            : OdbcStmt{stmt}{
            std::cout << "Fetcher create\n";
        }
        Fetcher( const Fetcher & stmt) 
            : OdbcStmt{stmt}{
            std::cout << "Fetcher copy\n";
        }
        Fetcher( Fetcher && stmt) 
            : OdbcStmt{stmt}{
            std::cout << "Fetcher move\n";
        }
        ~Fetcher(){
            std::cout << "delete Fetcher\n";
        }

        class FetchHelper{
            const int       MAX_DATA_SIZE = 1024;
            SQLHSTMT        stmt  = SQL_NULL_HSTMT;
            int             index = 1;
        public:
            FetchHelper() = default;
            FetchHelper(SQLHSTMT stmt) 
                : stmt(stmt) {
            }
            std::string getString(){
                char buffer[MAX_DATA_SIZE] = "";
                SQLRETURN status;
                SQLLEN len = 0;
                status = SQLGetData (stmt, 
                                    index++, 
                                    SQL_C_CHAR, 
                                    buffer, 
                                    sizeof(buffer), 
                                    (SQLLEN *)&len);
                if(status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
                }
                return buffer;
            }
            long getLong(){
                long buffer = 0;
                SQLRETURN status;
                SQLLEN len = 0;
                
                status = SQLGetData (stmt, 
                                    index++, 
                                    SQL_C_SLONG, 
                                    &buffer, 
                                    sizeof(buffer), 
                                    (SQLLEN *)&len);
                if(status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
                }
                return buffer;
            }
            short getShort(){
                short buffer = 0;
                SQLRETURN status;
                SQLLEN len = 0;
                
                status = SQLGetData (stmt, 
                                    index++, 
                                    SQL_C_SSHORT, 
                                    &buffer, 
                                    sizeof(buffer), 
                                    (SQLLEN *)&len);
                if(status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
                }
                return buffer;
            }
        };

        void
        fetch(int close_policy = SQL_CLOSE){
            SQLFreeStmt(stmt, close_policy);
        }

        template<typename Return_Ty>
        std::vector<Return_Ty> 
        fetch(std::function<Return_Ty(FetchHelper)> help, int initial_buf_size = 1, int close_policy = SQL_CLOSE){
            if(stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }

            SQLRETURN status;
            SQLSMALLINT col;
            std::cout << "start SQLNumResultCols error\n";
            status = SQLNumResultCols(stmt, &col);
            std::cout << "end SQLNumResultCols error status:" << status << std::endl;
            if( status != SQL_SUCCESS){
                std::cout << "SQLNumResultCols error\n";
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt); // no dbc error;
            }
            std::cout << "SQLNumResultCols vector resrevere start\n";
            std::vector<Return_Ty> results;
            results.reserve(initial_buf_size);

            std::cout << "SQLFetch start\n";
            if(col > 0 && help != nullptr){
                FetchHelper helper{stmt};
                while((status = SQLFetch(stmt)) != SQL_NO_DATA){
                    if(col > 0 && status != SQL_NO_DATA){
                        results.push_back(std::move(help(helper)));
                    }
                }
            }

            std::cout << "SQLFreeStmt start\n";

            SQLFreeStmt(stmt, close_policy);

            return results;
        }

    };
}