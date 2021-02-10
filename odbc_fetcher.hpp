#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcFetcher{
    private:
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

 

        unsigned short int
        getNumResultCols(){
            SQLSMALLINT col;

            SQLRETURN status = SQLNumResultCols(stmt->stmt, &col);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status); // no dbc error;
            }

            return col;
        }
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


        template<typename Return_Ty>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper )> help) {
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }

            SQLRETURN status;
            SQLSMALLINT col;

            status = SQLNumResultCols(stmt->stmt, &col);
            // std::cout << "SQLNumResultCols : status" << status << ", col : " << col << std::endl;
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status); // no dbc error;
            }

            std::vector<Return_Ty> results;
            if(col > 0)
                results.reserve(col);

            if(col > 0 && help != nullptr){
                
                FetchHelper helper(stmt->stmt);
                while((status = SQLFetch(stmt->stmt)) == SQL_SUCCESS){
                    if(col > 0 && status != SQL_NO_DATA){
                        results.push_back(std::move(help(helper)));
                    }
                }
            }

            status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
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