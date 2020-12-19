#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <sql.h>
#include <sqlext.h>

namespace odbctemplate
{
    class Fetcher{
    private:
        SQLHSTMT stmt = SQL_NULL_HSTMT;
    public:

        Fetcher() = default;
        Fetcher(
            SQLHSTMT stmt) 
            : stmt(stmt){
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
                std::cout << "getString:" << buffer << ", len:" << len << ", status: " << status << std::endl;
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
                std::cout << "getLong:" << buffer << ", len:" << len << ", status: " << status << std::endl;
                if(status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt);
                }
                return buffer;
            }
            short getShort(){
                std::cout << "short?\n";
                short buffer = 0;
                SQLRETURN status;
                SQLLEN len = 0;
                
                status = SQLGetData (stmt, 
                                    index++, 
                                    SQL_C_SSHORT, 
                                    &buffer, 
                                    sizeof(buffer), 
                                    (SQLLEN *)&len);
                std::cout << "getShort:" << buffer << ", len:" << len << ", status: " << status << std::endl;
                if(status != SQL_SUCCESS){
                    std::cout << "getShort status : " << status << std::endl;
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
                odbctemplate::OdbcError::Throw(); // no dbc error;
            }
                

            SQLRETURN status;
            SQLSMALLINT col;
            status = SQLNumResultCols(stmt, &col);
            std::cout << "SQLNumResultCols status: " << status << ", col : " << col << std::endl;
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt); // no dbc error;
            }

            std::vector<Return_Ty> results;
            results.reserve(initial_buf_size);

            if(col > 0 && help != nullptr){
                FetchHelper helper{stmt};
                while((status = SQLFetch(stmt)) != SQL_NO_DATA){
                    if(col > 0 == status != SQL_NO_DATA){
                        results.push_back(std::move(help(helper)));
                    }
                }
            }

            SQLFreeStmt(stmt, close_policy);

            return results;
        }


  

    };
}