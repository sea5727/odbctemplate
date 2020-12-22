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
            SQLFreeStmt(stmt->stmt, close_policy);
        }

        template<typename Return_Ty>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper)> help, int initial_buf_size = 1, int close_policy = SQL_CLOSE) {
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }

            SQLRETURN status;
            SQLSMALLINT col;
            status = SQLNumResultCols(stmt->stmt, &col);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt); // no dbc error;
            }
            std::vector<Return_Ty> results;

            if(col > 0 && help != nullptr){
                FetchHelper helper{stmt->stmt};
                while((status = SQLFetch(stmt->stmt)) == SQL_SUCCESS){
                    if(col > 0 && status != SQL_NO_DATA){
                        results.push_back(std::move(help(helper)));
                    }
                }
            }

            status = SQLFreeStmt(stmt->stmt, close_policy);
            //std::cout << __func__ << ": SQLFreeStmt stuatus : " << status << std::endl;
            return results;
        }

        template<typename Return_Ty>
        Return_Ty
        fetchOne(std::function<Return_Ty(FetchHelper)> help, int close_policy = SQL_CLOSE)  {
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }

            SQLRETURN status;
            SQLSMALLINT col;
            status = SQLNumResultCols(stmt->stmt, &col);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt); // no dbc error;
            }
            Return_Ty results;

            //std::cout << "=========== fetch start =============== \n";
            if(col > 0 && help != nullptr){
                FetchHelper helper{stmt->stmt};
                status = SQLFetch(stmt->stmt);
                //std::cout << "SQLFetch status : " << status << std::endl;
                if(status == SQL_SUCCESS){
                    if(col > 0 && status != SQL_NO_DATA){
                        results = help(helper);
                        //std::cout << "results : " << results << std::endl;
                    }
                }
            }
            //std::cout << "=========== fetch end =============== \n";

            status = SQLFreeStmt(stmt->stmt, close_policy);
            //std::cout << __func__ << ": SQLFreeStmt stuatus : " << status << std::endl;
            //std::cout << "return results : " << results << std::endl;
            return results;
        }
    };

    
}