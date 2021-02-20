#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{

    class BindColHelper{
    public:
        friend class OdbcpreparedStmt;
        friend class OdbcFetcher;
        const int       MAX_DATA_SIZE = 1024;
        int             index = 1;
        SQLHSTMT        stmt  = SQL_NULL_HSTMT;
    private:
        BindColHelper() = default;
        BindColHelper(SQLHSTMT stmt) 
            : stmt(stmt) {
        }
    public:
        void
        setBindColLongNotNull(int64_t * buffer){            
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_LONG, buffer, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setBindColLongNullable(NullInt64 * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_LONG, &buffer->Int64, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }

        void
        setBindColShortNotNull(int32_t * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_SHORT, buffer, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setBindColShortNullable(NullInt32 * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_SHORT, &buffer->Int32, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        template<int N>
        void
        setBindColStringNotNull(char (&buffer)[N]){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_CHAR, buffer, N, NULL);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        template<int N>
        void
        setBindColStringNullable(NullChar<N> * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_CHAR, buffer->Char, N, &buffer->len);
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
            // // u_char name[32];
            // // SQLSMALLINT namelen;
            // // SQLULEN colsize;
            // // SQLSMALLINT decimal;
            // // SQLSMALLINT datatype;
            // // SQLSMALLINT nullable;
            // // SQLRETURN status2 = SQLDescribeCol(stmt , index, name, sizeof(name), &namelen, &datatype, &colsize, &decimal, &nullable);
            // // printf("%d SQLDescribeCol status:%d, name:%s, namelen:%d, datatype:%d, colsize:%d, decimal:%d, nullable:%d\n", 
            // //     index,
            // //     status2, 
            // //     name, 
            // //     namelen, 
            // //     datatype,
            // //     colsize,
            // //     decimal,
            // //     nullable) ;
        NullString
        getNullString(){
            NullString ret;
            char buffer[MAX_DATA_SIZE] = "";
            SQLRETURN status;
            SQLLEN len = 0;
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_CHAR, 
                                buffer, 
                                sizeof(buffer), 
                                (SQLLEN *)&ret.len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            if(ret.len > 0)
                ret.String.assign(buffer, ret.len);
            return ret;
        }
        String
        getString(){
            String ret;
            char buffer[MAX_DATA_SIZE] = "";
            SQLRETURN status;
            SQLLEN len = 0;
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_CHAR, 
                                buffer, 
                                sizeof(buffer), 
                                NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            ret.String.assign(buffer);
            return ret;
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

        NullInt64
        getNullInt64(){
            odbctemplate::NullInt64 ret;
            SQLRETURN status;
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_SLONG, 
                                &ret.Int64, 
                                sizeof(ret.Int64), 
                                (SQLLEN *)&ret.len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            return ret;
        }
        
        Int64
        getInt64(){
            Int64 ret;
            long buffer = 0;
            SQLRETURN status;
            SQLLEN len = 0;
            
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_SLONG, 
                                &ret.Int64, 
                                sizeof(ret.Int64), 
                                NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            return ret;
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

        Int32
        getInt32(){
            Int32 ret;
            short buffer = 0;
            SQLRETURN status;
            SQLLEN len = 0;
            
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_SSHORT, 
                                &ret.Int32, 
                                sizeof(ret.Int32), 
                                NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            return ret;
        }

        NullInt32
        getNullInt32(){
            NullInt32 ret;
            short buffer = 0;
            SQLRETURN status;
            SQLLEN len = 0;
            
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_SSHORT, 
                                &ret.Int32, 
                                sizeof(ret.Int32), 
                                (SQLLEN *)&ret.len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            return ret;
        }
    };

}