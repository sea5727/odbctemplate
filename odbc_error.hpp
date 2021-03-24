#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcError : public std::exception{
    public :
        std::string code;
        std::string msg;
        OdbcError(){
            code = "-1";
            msg = "unknown error";
        } 
        OdbcError(const SQLRETURN status){
            if(status == -2){
                code = "-2";
                msg = "invalid handle state";
            }
        }        
        OdbcError(const std::string & message){
            code = "-1";
            msg = message;
        }
        OdbcError(const Ty_Result & error){
            code = std::get<CODE>(error);
            msg = std::get<ERROR_MESSAGE>(error);
        }
        OdbcError( 
            SQLSMALLINT type, 
            SQLHANDLE handle,
            SQLRETURN status){
                if(status == SQL_INVALID_HANDLE){
                    code = -2;
                    switch(type){
                        case SQL_HANDLE_ENV:
                            msg = "[ENV]: SQL_INVALID_HANDLE";
                        break;
                        case SQL_HANDLE_DBC:
                            msg = "[DBC]: SQL_INVALID_HANDLE";
                        break;
                        case SQL_HANDLE_STMT:
                            msg = "[STMT]: SQL_INVALID_HANDLE";
                        break;
                        default:
                            msg = "[Unkown]: SQL_INVALID_HANDLE";
                        break;
                    }
                }
                else {
                    auto error = get_odbc_error(type, handle);
                    code = std::get<CODE>(error);
                    msg = std::get<ERROR_MESSAGE>(error);
                }

        }
        const char * code() const noexcept {
            return code.c_str();
        }
        const char * what() const noexcept override {
            std::stringstream ss;
            ss << "[" << code << "] " << msg;
            
            return ss.str().c_str();
        }


        static void 
        Throw(const std::string & error) {
            throw OdbcError{error};
        };
        static void 
        Throw(Ty_Result error) {
            throw OdbcError{error};
        };
        static void 
        Throw( 
            SQLSMALLINT type, 
            SQLHANDLE handle,
            SQLRETURN status) {
            if(status == SQL_NO_DATA){
                throw OdbcError{"SQL_NO_DATA"};
            }
            throw OdbcError{type, handle, status};
        };

        static
        Ty_Result 
        get_odbc_error(
            SQLSMALLINT type,
            SQLHANDLE handle ){

            SQLINTEGER      native;
            char            state[32 ] = {0,};
            char            text [512] = {0,};
            SQLSMALLINT     len;
            SQLRETURN       ret;
            SQLLEN numRecs = 0;
            SQLGetDiagField(type, handle, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
            for(int i = 1 ; i <= numRecs; ++i){
                ret = SQLGetDiagRec(type, 
                                    handle, 
                                    i, 
                                    (SQLCHAR *)state, 
                                    &native, 
                                    (SQLCHAR *)text,
                                    sizeof(text), 
                                    &len );
                // std::cout << "state:" << state << "text:" << text << std::endl;
                if(ret != SQL_SUCCESS)
                    break;
                
                return {state, text};
                
            }
            return {"-1", "unknown error"};
        }
    };
}