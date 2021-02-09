#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcStmt{
    public:
        SQLHSTMT stmt = SQL_NULL_HSTMT;
    public:
        explicit OdbcStmt() = default;
        explicit OdbcStmt(SQLHSTMT stmt) 
            : stmt(stmt){
        }
        explicit OdbcStmt(const OdbcStmt & copy) 
            : stmt(copy.stmt){
        }
        explicit OdbcStmt(OdbcStmt && move) 
            : stmt(move.stmt){
            move.stmt = SQL_NULL_HSTMT;
        }
        ~OdbcStmt(){
            if(stmt != SQL_NULL_HSTMT){
                SQLRETURN status = SQLFreeStmt(stmt, SQL_DROP);
                stmt = SQL_NULL_HSTMT;
            }                
        }
    };
}