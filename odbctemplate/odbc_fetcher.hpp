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
    class OdbcFetcher{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        void
        print_mystmt(){
            ////std::cout << "print_mystmt OdbcFetcher, " << stmt->stmt << std::endl;
        }

        explicit OdbcFetcher() = default;
        explicit OdbcFetcher(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{
            ////std::cout << "OdbcFetcher create\n";
        }
        explicit OdbcFetcher(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} {
            ////std::cout << "OdbcFetcher create..\n";
        }
        explicit OdbcFetcher(const OdbcFetcher & copy) 
            : stmt{copy.stmt}{
            ////std::cout << "OdbcFetcher copy create\n";
        }
        explicit OdbcFetcher(OdbcFetcher && move) 
            : stmt{std::move(move.stmt)}{
            ////std::cout << "OdbcFetcher move create\n";
        }
        ~OdbcFetcher(){
            ////std::cout << "delete OdbcFetcher\n";
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
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
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
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
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
                    odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
                }
                return buffer;
            }
        };

        template<typename Return_Ty>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper)> help) {
            return fetch<Return_Ty, 1>(help);
        }

        template<typename Return_Ty, int InitialBufferSize>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper)> help) {
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }
            if(InitialBufferSize <= 0){
                odbctemplate::OdbcError::Throw("invalid initial buffer size"); // no dbc error;
            }


            SQLRETURN status;
            SQLSMALLINT col;

            status = SQLNumResultCols(stmt->stmt, &col);
            // std::cout << "SQLNumResultCols : status" << status << ", col : " << col << std::endl;
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status); // no dbc error;
            }
            if(InitialBufferSize <= 0){
                odbctemplate::OdbcError::Throw("buffer initialize fail [invalid size]"); 
            }

            std::vector<Return_Ty> results;
            results.reserve(InitialBufferSize);

            if(col > 0 && help != nullptr){
                
                FetchHelper helper{stmt->stmt};
                while((status = SQLFetch(stmt->stmt)) == SQL_SUCCESS){
                    // std::cout << "SQLFetch status : " << status << std::endl;
                    if(col > 0 && status != SQL_NO_DATA){
                        results.push_back(std::move(help(helper)));
                    }
                }
                // std::cout << "end SQLFetch status : " << status << std::endl;
            }

            status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
            //std::cout << __func__ << ": SQLFreeStmt stuatus : " << status << std::endl;
            return results;
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

        long
        getRowCount(){ // Insert, Update, Delete
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }
  
            SQLRETURN status;
            
            // status = SQLFreeStmt(stmt->stmt, SQL_RESET_PARAMS);
            // std::cout << "getSuccessRow return  : " << status << std::endl;
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