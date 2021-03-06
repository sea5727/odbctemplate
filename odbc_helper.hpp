#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class BindColHelper{
    public:
        friend class OdbcpreparedStmt;
        friend class OdbcFetcher;
    private:
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
        setInt64NotNull(Int64 * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_LONG, &buffer->Value, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setInt64Nullable(NullInt64 * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_LONG, &buffer->Value, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }

        void
        setInt16NotNull(Int16 * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_SHORT, &buffer->Value, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setInt16Nullable(NullInt16 * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_SHORT, &buffer->Value, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }

        template<int N>
        void
        setCharNotNull(Char<N> *buffer){
            
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_CHAR, buffer->Value, N, NULL);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        template<int N>
        void
        setCharNullable(NullChar<N> * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_CHAR, buffer->Value, N, &buffer->len);
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

        inline
        void
        Clear(){
            index = 1;
        }


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
                ret.Value.assign(buffer, ret.len);
            return ret;
        }

        String
        getString(){
            String ret;
            char buffer[MAX_DATA_SIZE] = "";
            SQLRETURN status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_CHAR, 
                                buffer, 
                                sizeof(buffer), 
                                NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            ret.Value.assign(buffer);
            return ret;
        }

        char *
        getStringToBuffer(char * buffer, size_t size){
            SQLRETURN status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_CHAR, 
                                buffer, 
                                size, 
                                NULL);
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

        NullInt64
        getNullInt64(){
            odbctemplate::NullInt64 ret;
            SQLRETURN status;
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_SLONG, 
                                &ret.Value, 
                                sizeof(ret.Value), 
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
                                &ret.Value, 
                                sizeof(ret.Value), 
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

        Int16
        getInt16(){
            Int16 ret;
            short buffer = 0;
            SQLRETURN status;
            SQLLEN len = 0;
            
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_SSHORT, 
                                &ret.Value, 
                                sizeof(ret.Value), 
                                NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            return ret;
        }

        NullInt16
        getNullInt16(){
            NullInt16 ret;
            short buffer = 0;
            SQLRETURN status;
            SQLLEN len = 0;
            
            status = SQLGetData (stmt, 
                                index++, 
                                SQL_C_SSHORT, 
                                &ret.Value, 
                                sizeof(ret.Value), 
                                (SQLLEN *)&ret.len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
            return ret;
        }
    };

}