#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcDbc{
    public:
        SQLHDBC dbc = SQL_NULL_HDBC;

        explicit OdbcDbc() = default;
        explicit OdbcDbc(SQLHDBC dbc) 
            : dbc(dbc){
            // std::cout << "OdbcDbc create..\n";
        }
        explicit OdbcDbc(const OdbcDbc & copy) 
            : dbc(dbc){
            // std::cout << "OdbcStmt copy create..\n";
        }
        explicit OdbcDbc(OdbcDbc && move) 
            : dbc(dbc){
            dbc = move.dbc;
            move.dbc = SQL_NULL_HDBC;
            // std::cout << "OdbcStmt move create..\n";
        }
        ~OdbcDbc(){
            // std::cout << "~OdbcDbc\n";
            if(dbc != SQL_NULL_HDBC){
                SQLRETURN status;
                status = SQLDisconnect(dbc);
                status = SQLFreeConnect(dbc);
                dbc = SQL_NULL_HDBC;
            }                
        }
 
    public:



    };
}