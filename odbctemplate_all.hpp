#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <tuple>
#include <sql.h>
#include <sqlext.h>
#include <functional>
#include <typeinfo> 
#include <string.h>
#include <vector>
#include <exception>
#include <sstream>
#include <utility>
#include <sqlext.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

namespace odbctemplate
{
    using Ty_Result = std::tuple<
        std::string /*error_code*/, 
        std::string /*error_desc*/>;
    enum JackPotIndex{
        CODE = 0,
        ERROR_MESSAGE,
    };
}




namespace odbctemplate
{
    class OdbcError : public std::exception{
    public :
        std::string error_code;
        std::string error_msg;
        OdbcError(){
            error_code = "-1";
            error_msg = "unknown error";
        } 
        OdbcError(const SQLRETURN status){
            if(status == -2){
                error_code = "-2";
                error_msg = "invalid handle state";
            }
        }        
        OdbcError(const std::string & message){
            error_code = "-1";
            error_msg = message;
        }
        OdbcError(const Ty_Result & error){
            error_code = std::get<CODE>(error);
            error_msg = std::get<ERROR_MESSAGE>(error);
        }
        OdbcError( 
            SQLSMALLINT type, 
            SQLHANDLE handle,
            SQLRETURN status){
                if(status == SQL_INVALID_HANDLE){
                    error_code = -2;
                    switch(type){
                        case SQL_HANDLE_ENV:
                            error_msg = "[ENV]: SQL_INVALID_HANDLE";
                        break;
                        case SQL_HANDLE_DBC:
                            error_msg = "[DBC]: SQL_INVALID_HANDLE";
                        break;
                        case SQL_HANDLE_STMT:
                            error_msg = "[STMT]: SQL_INVALID_HANDLE";
                        break;
                        default:
                            error_msg = "[Unkown]: SQL_INVALID_HANDLE";
                        break;
                    }
                }
                else {
                    auto error = get_odbc_error(type, handle);
                    error_code = std::get<CODE>(error);
                    error_msg = std::get<ERROR_MESSAGE>(error);
                }

        }
        const char * code() const noexcept {
            return error_code.c_str();
        }
        const char * what() const noexcept override {
            std::stringstream ss;
            ss << "[" << error_code << "] " << error_msg;
            
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
                std::cout << "state:" << state << "text:" << text << std::endl;
                if(ret != SQL_SUCCESS)
                    break;
                
                return {state, text};
                
            }
            return {"-1", "unknown error"};
        }
    };
}

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

namespace odbctemplate
{
    class OdbcManager{
    private:
        OdbcManager() = default;
        ~OdbcManager(){
            SQLFreeEnv(env);
        }

    public:
        SQLHENV env = SQL_NULL_HENV;
        static OdbcManager & getInstance(){
            static OdbcManager * instance = nullptr;
            if(instance == nullptr){
                instance = new OdbcManager;
            }
            return *instance;
        }

        OdbcManager &
        init_odbc(){

            odbctemplate::OdbcManager & pm = odbctemplate::OdbcManager::getInstance();

            if(env == SQL_NULL_HENV){
                SQLRETURN status = 0;
                status = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
                if(status != SQL_SUCCESS){
                    odbctemplate::OdbcError::Throw("init_odbc SQLAllocHandle SQL_HANDLE_ENV fail");
                }
                status = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
                if(status != SQL_SUCCESS){
                    auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_ENV, env);
                    SQLFreeEnv(env);
                    env = SQL_NULL_HENV;
                    odbctemplate::OdbcError::Throw(error);
                }
            }
            return pm;
        }




    };

}

namespace odbctemplate
{
    class Nullable {
    public:
        Nullable() {
            len = -1;
        }
        long len;

        inline
        bool
        Valid(){
            return len != -1;
        }
    };

    class NullString : public Nullable {
    public:
        std::string Value;
    };
    template<int N>
    class NullChar : public Nullable {
    public:
        char Value[N];
    };
    template<>
    class NullChar<1> : public Nullable {
    public:
        char Value;
    };
    class NullInt64 : public Nullable {
    public:
        int64_t Value;
    };
    class NullUInt64 : public Nullable {
    public:
        uint64_t Value;
    };
    class NullInt32 : public Nullable {
    public:
        int32_t Value;
    };
    class NullUInt32 : public Nullable {
    public:
        uint64_t Value;
    };
    class NullBool : public Nullable {
    public:
        bool Value;
    };
    class NullFloat64 : public Nullable {
    public:
        float Value;  
    };

    template<int N>
    class Char {
    public:
        char Value[N];
    };
    template<>
    class Char<1> {
    public:
        char Value;
    };
    class String {
    public:
        std::string Value;    
    };
    class Int64 {
    public:
        int64_t Value;
    };
    class UInt64 {
    public:
        uint64_t Value;
    };
    class Int32 {
    public:
        int32_t Value;
    };
    class UInt32 {
    public:
        uint32_t Value;
    };
    class Bool {
    public:
        bool Value;
    };
    class Float64 {
    public:
        float Value;
    };

}

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
        setInt32NotNull(Int32 * buffer){
            SQLRETURN status = SQLBindCol(stmt, index++, SQL_C_SHORT, &buffer->Value, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt, status);
            }
        }
        void
        setInt32Nullable(NullInt32 * buffer){
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

        Int32
        getInt32(){
            Int32 ret;
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

        NullInt32
        getNullInt32(){
            NullInt32 ret;
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

namespace odbctemplate
{

    class OdbcFetcher{
    public:
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

 
        /**
         * @brief Get the Num Result Cols
         * 
         * @return unsigned short int 
         */
        unsigned short int
        getNumResultCols(){
            SQLSMALLINT col = 0;

            SQLRETURN status = SQLNumResultCols(stmt->stmt, &col);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status); // no dbc error;
            }

            return col;
        }

        /**
         * @brief fetch 데이터를 결과 데이터에 bind해주는 helper용 함수.
         * 사용자는 help 함수에 binding 해주는 함수를 작성해야한다.
         * 
         * @param help 
         * @return OdbcpreparedStmt 
         */
        OdbcFetcher
        bindResultCol(std::function<void(BindColHelper & )> help){
            BindColHelper helper{stmt->stmt};
            help(helper);
            return *this;
        }

        OdbcFetcher
        bindCol(const u_short index, Int64 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_LONG, &buffer->Value, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        OdbcFetcher
        bindCol(const u_short index, NullInt64 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_LONG, &buffer->Value, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        OdbcFetcher
        bindCol(const u_short index, Int32 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_SHORT, &buffer->Value, 0, NULL);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        OdbcFetcher
        bindCol(const u_short index, NullInt32 * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_SHORT, &buffer->Value, 0, &buffer->len);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        template<int N>
        OdbcFetcher
        bindCol(const u_short index, Char<N> *buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, NULL);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        template<int N>
        OdbcFetcher
        bindCol(const u_short index, NullChar<N> * buffer){
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, &buffer->len);
             if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
      
        OdbcFetcher
        getData(const u_short index, NullFloat64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, 
                                index, 
                                SQL_C_FLOAT, 
                                &buffer->Value, 
                                sizeof(buffer->Value), 
                                (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, Float64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_FLOAT, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, NullInt32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SSHORT, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, Int32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SSHORT, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }
        
        OdbcFetcher
        getData(const u_short index, NullUInt32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_USHORT, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, UInt32 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_USHORT, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, NullUInt64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_ULONG, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, UInt64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_ULONG, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, NullInt64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SLONG, &buffer->Value, sizeof(buffer->Value), (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        OdbcFetcher
        getData(const u_short index, Int64 * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_SLONG, &buffer->Value, sizeof(buffer->Value), NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        template<int N>
        OdbcFetcher
        getData(const u_short index, NullChar<N> * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, (SQLLEN *)&buffer->len);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }

        template<int N>
        OdbcFetcher
        getData(const u_short index, Char<N> * buffer){
            SQLRETURN status;
            
            status = SQLGetData (stmt->stmt, index, SQL_C_CHAR, buffer->Value, N, NULL);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return *this;
        }


        /**
         * @brief fetch를 1회 호출
         * 
         * @return true 
         * @return false 
         */
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

        /**
         * @brief 쿼리 결과를 fetch 합니다. ( 주로 select 결과 )
         * 
         * @tparam Return_Ty 쿼리에 대응하는 class 또는 struct 타입이어야 합니다.
         * @param help  Return_Ty를 return 하는 헬퍼 함수여야합니다.
         * @param N vector의 초기 크기. performance param
         * @return std::vector<Return_Ty> 
         */
        template<typename Return_Ty>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper &)> && help, int N) {
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }

            SQLRETURN status;
            auto col = getNumResultCols();

            std::vector<Return_Ty> results;
            if(N > 0)
                results.reserve(N);

            if(col > 0 && help != nullptr){
                FetchHelper helper(stmt->stmt);
                while((status = SQLFetch(stmt->stmt)) == SQL_SUCCESS){
                    helper.Clear();    
                    if(col > 0 && status != SQL_NO_DATA){
                        results.push_back(std::move(help(helper)));
                    }
                }
            }

            status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
            return results;
        }

        template<typename Return_Ty>
        std::vector<Return_Ty>
        fetch(std::function<Return_Ty(FetchHelper &)> && help) {
            return fetch<Return_Ty>(std::move(help), 0);
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

        /**
         * @brief update/insert/delete 의 쿼리 결과 count
         * 
         * @return long 
         */
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

namespace odbctemplate
{
    class OdbcpreparedStmt{
        friend class OdbcpreparedUtil;
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcpreparedStmt() = default;
        explicit OdbcpreparedStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{ }
        explicit OdbcpreparedStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} { }
        ~OdbcpreparedStmt(){ }
 
    public:
        /**
         * @brief fetch 데이터를 결과 데이터에 bind해주는 helper용 함수.
         * 사용자는 help 함수에 binding 해주는 함수를 작성해야한다.
         * 
         * @param help 
         * @return OdbcpreparedStmt 
         */
        OdbcpreparedStmt
        bindResultCol(std::function<void(BindColHelper & )> help){
            BindColHelper helper{stmt->stmt};
            help(helper);
            return *this;
        }

        template <typename Param1, typename... Params>
        OdbcFetcher 
        bindExecute(const Param1 & p1, const Params&... rest){
            // std::cout << "p1 : " << (void *)&p1 << ", type:" << typeid(p1).name() << std::endl;
            bindForParams(1, p1, rest...);
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher{stmt};
        }

        OdbcFetcher 
        Execute(){
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher{stmt};
        }
    
    private:
        template< unsigned N >
        void bind(const int index, const char (&arr)[N] ){
            // std::cout << "bind const char (&arr)[N].." << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)arr,
                                    strlen(arr),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }


        void bind(const int index, const std::string & param) {
            // std::cout << "bind const std::string.." << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)param.c_str(),
                                    param.length(),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void bind(const int index, char * const &arr ){
            // std::cout << "bind const char *arr.." << (void *)&arr << std::endl;
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)arr,
                                    strlen(arr),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }        

        void bind(const int index, const uint & param) {
            std::cout << "bind const uint.." << std::endl;
            SQLRETURN status = 1;

            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_ULONG,
                                    SQL_INTEGER,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void bind(const int index, const int & param) {
            SQLRETURN status = 1;

            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_SLONG,
                                    SQL_INTEGER,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        template < typename Params>
        void bindForParams(int index, const Params& param){
            // // std::cout << "bindForParams p1 : " << (void *)&param << ", type:" << typeid(param).name() << std::endl;
            bind(index, param);
        }
        template <typename Param1, typename... Params>
        void bindForParams(int index, const Param1 & param, const Params&... rest){
            // // std::cout << "bindForParams p1 : " << (void *)&param << ", type:" << typeid(param).name() << std::endl;
            bind(index, param);
            bindForParams(++index, rest...);
            return;
        }
    };


}

namespace odbctemplate
{
    class OdbcResetedStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcResetedStmt() = default;
        explicit OdbcResetedStmt(SQLHSTMT stmt) 
            : stmt(std::make_shared<OdbcStmt>(stmt)) { }
 
    public:
        /**
         * @brief preparedStmt 핸들러를 얻는다.
         * 
         * @param query 
         * @return OdbcpreparedStmt 
         */
        OdbcpreparedStmt
        preparedStmt(const std::string & query){
            SQLRETURN status = SQLPrepare( stmt->stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcpreparedStmt(stmt);
        }
        
        /**
         * @brief 직접 쿼리를 한다. ( bind 불가 )
         * 
         * @param query 
         * @return OdbcFetcher 
         */
        OdbcFetcher
        directExecute(const std::string & query){
            SQLRETURN status = SQLExecDirect(stmt->stmt, (SQLCHAR *)query.c_str(), SQL_NTS);
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher(stmt);
        }

        /**
         * @brief preparedStmt 핸들러로 쿼리를 한다 ( bind 불가 )
         * 
         * @param query 
         * @return OdbcFetcher 
         */
        OdbcFetcher 
        preparedExecute(const std::string & query){
            return preparedStmt(query).Execute();
        }

        /**
         * @brief preparedStmt 핸들러로 쿼리를 한다 ( bind 가능 )
         * 
         * @tparam Param1 
         * @tparam Params 
         * @param query 
         * @param p1 
         * @param rest 
         * @return OdbcFetcher 
         */
        template <typename Param1, typename... Params>
        OdbcFetcher 
        preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
            return preparedStmt(query).bindExecute(p1, rest...);
        }

    };
}

namespace odbctemplate
{
    class OdbcConnectBuilder;
    class OdbcConnect{
        friend class OdbcConnectBuilder;
        friend class OdbcConnectCtl;
    public:
        std::shared_ptr<OdbcDbc> dbc;
    public:
        explicit OdbcConnect() = default;
        static OdbcConnectBuilder builder();
    
    public:

        /**
         * @brief BASE STMT 핸들러 
         * SQLAllocHandle 를 호출
         * 
         * @return OdbcResetedStmt 
         */
        OdbcResetedStmt 
        allocStmt() {
            SQLRETURN status = 0;
            SQLHDBC stmt = SQL_NULL_HSTMT;

            status = SQLAllocHandle(SQL_HANDLE_STMT, dbc->dbc, &stmt);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }

            return OdbcResetedStmt(stmt);
        }


        /**
         * @brief preparedStmt 핸들러를 얻는다.
         * 
         * @param query 
         * @return OdbcpreparedStmt 
         */
        OdbcpreparedStmt
        preparedStmt(const std::string & query){
            return allocStmt().preparedStmt(query);
        }

        /**
         * @brief preparedStmt 핸들러로 직접 쿼리를 한다. ( bind 불가 )
         * 
         * @param query 
         * @return OdbcpreparedStmt 
         */
        OdbcFetcher 
        preparedExecute(const std::string & query){
            return allocStmt().preparedStmt(query).Execute();
        }


        /**
         * @brief prepared 핸들러로 직접 쿼리를 한다. ( bind 가능 )
         * 
         * @tparam Param1 
         * @tparam Params 
         * @param query 
         * @param p1 first bind variable
         * @param rest others bind variables
         * @return OdbcFetcher 
         */
        template <typename Param1, typename... Params>
        OdbcFetcher 
        preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
            // // std::cout << "p1 : " << (void *)&p1 << ", type:" << typeid(p1).name() << std::endl;
            return allocStmt().preparedStmt(query).bindExecute(p1, rest...);
        }

        /**
         * @brief 직접 쿼리를 한다. ( bind 불가 )
         * 
         * @param query 
         * @return OdbcFetcher 
         */
        OdbcFetcher 
        directExecute(const std::string & query){
            return allocStmt().directExecute(query);
        }

        void
        commit(){
            SQLRETURN status = SQLTransact(SQL_NULL_HENV, dbc->dbc, SQL_COMMIT);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }
        void
        rollback(){
            SQLRETURN status = SQLTransact(SQL_NULL_HENV, dbc->dbc, SQL_ROLLBACK);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }
    };
}

namespace odbctemplate
{
    class OdbcConnectionPool{
    private:
        std::string conn_info = "";
        unsigned int max_pool = 32;
        unsigned int min_pool = 4;
        unsigned int current_size = 4;
        std::vector<OdbcConnect> pool;
    public:
        OdbcConnectionPool() = default;
        OdbcConnectionPool(const std::string & conn_info, unsigned int min_pool, unsigned int max_pool)
            : conn_info(conn_info)
            , max_pool(max_pool) 
            , min_pool(min_pool)
            , current_size(min_pool) {
            
            //std::cout << "create\n";

            pool.reserve(max_pool);

            //std::cout << "reserve\n";

            for(unsigned int i = 0 ; i < min_pool ; ++i){
                //std::cout << "i:" << i << std::endl;
                // pool.emplace_back(OdbcConnect::get_connection(conn_info));
            }

            //std::cout << "end\n";
        }

        static
        OdbcConnectionPool
        get_connection_pool(const std::string & conn_info, unsigned int min_pool, unsigned int max_pool){
            return {conn_info, min_pool, max_pool};
        }

        OdbcConnect &
        get_connection_idx(const unsigned int index){
            if(index > max_pool)
                odbctemplate::OdbcError::Throw("Invalid Connection Pool index"); 
            //std::cout << "index : " << index << ", current_size:" << current_size << std::endl;

            if(current_size < index + 1){
                for(unsigned int i = current_size ; i < index + 1 ; ++i){
                    //std::cout << "i : " << i << std::endl;
                    // pool.emplace_back(OdbcConnect::get_connection(conn_info));
                }
                current_size = index + 1;
            }
            return pool[index]; // copy??
        }
        
        
        void
        put_connection(){
            
        }
        


    };
}

namespace odbctemplate
{
    class OdbcConnectCtl {
    public:
        static
        void 
        setLoginTimeout(OdbcConnect & conn, const long login_timeout){
            SQLRETURN status = SQLSetConnectAttr(conn.dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) login_timeout, 0);
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                odbctemplate::OdbcError::Throw(error);
            }
        }
        static
        void
        setAutocommit(OdbcConnect & conn, bool autocommit) {
            SQLRETURN status = SQLSetConnectAttr (conn.dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)autocommit, 0); // if set to a pointer, a failure occurs
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                odbctemplate::OdbcError::Throw(error);
            }
        }
        static
        void
        connectDb(OdbcConnect & conn, const std::string & dsn){
            static std::mutex mtx;
            mtx.lock();
            SQLRETURN status = SQLDriverConnect(conn.dbc->dbc, NULL, (SQLCHAR *)dsn.c_str(), dsn.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
            if (status != SQL_SUCCESS){
                mtx.unlock();
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                odbctemplate::OdbcError::Throw(error);
            }
            mtx.unlock();
        }

        /**
         * @brief Get the Autocommit object
         * 
         * @return true SQL_AUTOCOMMIT_ON 1
         * @return false SQL_AUTOCOMMIT_OFF 0
         */
        static
        bool
        getAutocommit(OdbcConnect & conn){
            SQLINTEGER value;
            SQLRETURN status = SQLGetConnectAttr(conn.dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return (bool)value; 
        }

        static
        bool
        getConnectionDead(OdbcConnect & conn) {
            SQLINTEGER 	value;
            SQLRETURN status = SQLGetConnectAttr(conn.dbc->dbc, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER) &value, 0, NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return (bool)value;
        }

        static
        long
        getLoginTimeout(OdbcConnect & conn){
            SQLINTEGER value;
            SQLRETURN status = SQLGetConnectAttr(conn.dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_DBC, conn.dbc->dbc, status);
            }
            return value;
        }

    };

}

namespace odbctemplate
{
    class OdbcConnectBuilder{
    private:
        OdbcConnect conn;
    public:
        explicit OdbcConnectBuilder() {
            auto & om = odbctemplate::OdbcManager::getInstance();
            om.init_odbc();

            SQLHDBC tmp_dbc;
            SQLRETURN status = SQLAllocHandle (SQL_HANDLE_DBC, om.env, &tmp_dbc);
            if (status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw("get_connection SQLAllocHandle SQL_HANDLE_DBC fail :" + std::to_string(status));
            }
            auto dbc = std::make_shared<OdbcDbc>(tmp_dbc);
            conn.dbc = dbc;
        }

        explicit OdbcConnectBuilder(OdbcConnect conn) 
            : conn(conn){

        }


        OdbcConnectBuilder &
        setDsn(const std::string & dsn) {
            //todo validation?
            OdbcConnectCtl::connectDb(conn, dsn);
            return *this;
        }


        OdbcConnectBuilder &
        setLoginTimeout(const long login_timeout){
            OdbcConnectCtl::setLoginTimeout(conn, login_timeout);
            return *this;
        }

        OdbcConnectBuilder & 
        setAutocommit(bool autocommit){
            OdbcConnectCtl::setAutocommit(conn, autocommit);
            return *this;
        }

        OdbcConnect &&
        build(){
            return std::move(conn);
        }



    };

    OdbcConnectBuilder 
    OdbcConnect::builder(){
        return OdbcConnectBuilder ();
    }

}


