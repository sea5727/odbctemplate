#pragma once

#include <exception>
#include <sstream>
#include "odbc_helper.hpp"

namespace odbctemplate
{
    class OdbcError : public std::exception{
    public :
        int error_code;
        std::string error_msg;
        OdbcError(){
            error_code = -1;
            error_msg = "unknown error";
        } 
        OdbcError(std::string & message){
            error_code = -1;
            error_msg = message;
        }
        OdbcError(const Ty_Result & error){
            error_code = std::get<CODE>(error);
            error_msg = std::get<ERROR_MESSAGE>(error);
        }
        OdbcError( 
            SQLSMALLINT type, 
            SQLHANDLE handle){
                auto error = get_odbc_error(type, handle);
                error_code = std::get<CODE>(error);
                error_msg = std::get<ERROR_MESSAGE>(error);
        }
        const int code() const noexcept {
            return error_code;
        }
        const char * what() const noexcept override {
            std::stringstream ss;
            ss << "[" << error_code << "] " << error_msg;
            
            return ss.str().c_str();
        }


        static void 
        Throw() {
            throw OdbcError{};
        };
        static void 
        Throw(const std::string & error) {
            throw OdbcError{};
        };
        static void 
        Throw(Ty_Result error) {
            throw OdbcError{error};
        };
        static void 
        Throw( 
            SQLSMALLINT type, 
            SQLHANDLE handle) {
            throw OdbcError{type, handle};
        };

        static
        Ty_Result 
        get_odbc_error(
            SQLSMALLINT type,
            SQLHANDLE handle ){

            int  e_code = -1;
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
                std::cout << "state:" << state << "text:" << text << std::endl;
                if(ret != SQL_SUCCESS)
                    break;
                
                return {std::stoi(state), text};
                
            }
            return {-1, "unknown error"};
        }
    };
}