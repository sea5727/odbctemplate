#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcDbc{
    public:
        SQLHDBC dbc = SQL_NULL_HDBC;

        explicit OdbcDbc() = default;
        explicit OdbcDbc(SQLHDBC dbc) 
            : dbc(dbc){}
        explicit OdbcDbc(const OdbcDbc & copy) 
            : dbc(copy.dbc){}
        explicit OdbcDbc(OdbcDbc && move) 
            : dbc(move.dbc){
            move.dbc = SQL_NULL_HDBC;
        }
        ~OdbcDbc(){
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