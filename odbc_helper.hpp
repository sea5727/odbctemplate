#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{

    class BindColHelper{
    public:
        friend class OdbcpreparedStmt;
        int             index = 1;
        SQLHSTMT        stmt  = SQL_NULL_HSTMT;
    private:
        BindColHelper() = default;
        BindColHelper(SQLHSTMT stmt) 
            : stmt(stmt) {
        }
    public:
        void
        setBindColLongNotNull(long * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_LONG, buffer, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setBindColLongNullable(long * buffer, long * len){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_LONG, buffer, 0, len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setBindColShortNotNull(long * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_SHORT, buffer, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setBindColShortNullable(long * buffer, long * len){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_SHORT, buffer, 0, len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setBindColStringNotNull(char * buffer, size_t buffer_size){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_CHAR, buffer, buffer_size, NULL);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
       void
        setBindColStringNullable(char * buffer, size_t buffer_size, long * len){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_CHAR, buffer, buffer_size, len);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        
    };

    class FetchHelper{
        const int       MAX_DATA_SIZE = 1024;
        SQLHSTMT        stmt  = SQL_NULL_HSTMT;
        int             index = 1;
    public:
        FetchHelper() = default;
        FetchHelper(SQLHSTMT stmt) 
            : stmt(stmt) {
        }

        std::string 
        getString(){
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
        long 
        getLong(){
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
        short 
        getShort(){
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

}