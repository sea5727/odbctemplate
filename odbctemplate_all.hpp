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

#pragma once

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
        std::string String;
    };

    template<int N>
    class NullChar : public Nullable {
    public:
        char Char[N];
    };

    class NullInt64 : public Nullable {
    public:
        int64_t Int64;
    };

    class NullInt32 : public Nullable {
    public:
        int32_t Int32;
    };

    class NullBool : public Nullable {
    public:
        bool Bool;
    };

    class NullFloat64 : public Nullable {
    public:
        float Float64;  
    };

    template<int N>
    class Char {
    public:
        char Char[N];
    };

    class String {
    public:
        std::string String;    
    };
    class Int64 {
    public:
        int64_t Int64;
    };

    class Int32 {
    public:
        int32_t Int32;
    };

    class Bool {
    public:
        bool Bool;
    };

    class Float64 {
    public:
        float Float64;
    };

}

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
            SQLSMALLINT col;

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
        fetch(std::function<Return_Ty(FetchHelper &)> help, int N) {
            if(stmt->stmt == SQL_NULL_HSTMT){
                odbctemplate::OdbcError::Throw("stmt is null"); // no dbc error;
            }

            SQLRETURN status;
            auto col = getNumResultCols();

            std::vector<Return_Ty> results;
            if(N > 0)
                results.reserve(N);

            if(col > 0 && help != nullptr){
                while((status = SQLFetch(stmt->stmt)) == SQL_SUCCESS){
                    FetchHelper helper(stmt->stmt);
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
        fetch(std::function<Return_Ty(FetchHelper &)> help) {
            return fetch<Return_Ty>(help, 0);
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
        bindExecute(const Param1 p1, const Params... rest){
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
        class templateString{
        public:
            templateString(){
            }
            templateString(const char * v)
                : string(v)
                , length(strlen(v)){
            }
            const char *string;
            int length;

        };


        void bind(const int index, const templateString & param) {
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    index,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)param.string,
                                    param.length,
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
            bind(index, param);
        }
        template <typename Param1, typename... Params>
        void bindForParams(int index, const Param1 & param, const Params&... rest){
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
            SQLRETURN status = SQLExecDirect(stmt->stmt, (SQLCHAR *)query.c_str(), query.length());
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
    private:
        std::shared_ptr<OdbcDbc> dbc;
    public:
        explicit OdbcConnect() = default;
        static OdbcConnectBuilder builder();
    
    private:

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

    public:
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
            SQLRETURN status = SQLDriverConnect(conn.dbc->dbc, NULL, (SQLCHAR *)dsn.c_str(), dsn.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
            if (status != SQL_SUCCESS){
                auto error = odbctemplate::OdbcError::get_odbc_error(SQL_HANDLE_DBC, conn.dbc->dbc);
                odbctemplate::OdbcError::Throw(error);
            }
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

