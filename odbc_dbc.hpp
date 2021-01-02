#pragma once

#include <iostream>
#include <string.h>
#include <sql.h>
#include <sqlext.h>

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
                std::cout << __func__ << ": SQLDisconnect SQLHDBC stuatus : " << status << std::endl;
                status = SQLFreeConnect(dbc);
                std::cout << __func__ << ": SQLFreeConnect SQLHDBC stuatus : " << status << std::endl;
                dbc = SQL_NULL_HDBC;
            }                
        }
 
    public:



    };
}