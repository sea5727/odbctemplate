#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>


int main(int, char**) {
    std::cout << "Hello, world!\n";

    std::string user = "ysh8361";
    std::string password = "ysh8361";
    std::string dsn ;
    SQLHDBC dbc = SQL_NULL_HDBC; 
    SQLHENV env = SQL_NULL_HENV;
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    if(!SQL_SUCCEEDED(ret)){
        std::cout << "SQLAllocHandle fail" << std::endl;
        
        
        return 0;
    }

    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
    if(!SQL_SUCCEEDED(ret)){
        std::cout << "SQLSetEnvAttr fail" << std::endl;
        return 0;
    }

    // // ret = SQLDisconnect(dbc);
    // // if(!SQL_SUCCEEDED(ret)){
    // //     std::cout << "SQLDisconnect fail" << std::endl;
    // // }

    // // ret = SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    // // if(!SQL_SUCCEEDED(ret)){
    // //     std::cout << "SQLFreeHandle fail" << std::endl;
    // // }
    // ret = SQLAllocConnect(env, &dbc);
    ret = SQLAllocHandle (SQL_HANDLE_DBC, env, &dbc);
    if (!SQL_SUCCEEDED(ret)){
        std::cout << "SQLFreeHandle fail" << std::endl;
        return 0;
    }



     // ============== connect start =================//
    long login_timeout = 1;

//    status=SQLSetConnectAttr(dbc, SQL_ATTR_LOGIN_TIMEOUT, OTL_RCAST(void*,OTL_SCAST(size_t,timeout)), 0);
    ret = SQLSetConnectAttr(dbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) &login_timeout, SQL_IS_POINTER);
    if (!SQL_SUCCEEDED(ret)){
        std::cout << "SQLSetConnectAttr fail" << std::endl;
        return 0;
    }

    ret = SQLSetConnectAttr (dbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER) &login_timeout, SQL_IS_POINTER);
    if (!SQL_SUCCEEDED(ret))
    {
        SQLFreeConnect(dbc);

        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] SQLSetConnectAttr[SQL_ATTR_CONNECTION_TIMEOUT]" << err_msg << ", " << err_num << std::endl;
        return 0;
    }

    long autocommit = 1;
    ret = SQLSetConnectAttr (dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)autocommit, 0);
    if (!SQL_SUCCEEDED(ret))
    {
        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] ret:" << ret << " SQLSetConnectAttr[SQL_ATTR_AUTOCOMMIT]" << err_msg << ", " << err_num << std::endl;

        SQLFreeConnect(dbc);
        return 0;
    }
    const char *conn_info = "DSN=TST_DB;";

    unsigned char outstr[1024] = "";
    short outstrlen = 0;
    ret = SQLDriverConnect(dbc, NULL, (SQLCHAR *)conn_info, SQL_NTS, NULL, NULL, NULL, NULL );

    if (!SQL_SUCCEEDED(ret))
    {
    
        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] SQLDriverConnect" << err_msg << ", " << err_num << std::endl;
        return 0;

    }


    unsigned long set_ltimeout = 120000;
    
    ret = SQLSetConnectAttr (dbc, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)&set_ltimeout, SQL_IS_POINTER); // * query timeout
    if (!SQL_SUCCEEDED(ret))
    {
        SQLFreeConnect(dbc);

        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] SQLSetStmtAttr " << err_msg << ", " << err_num << std::endl;
        return 0;
    }

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    // ret = SQLAllocStmt (dbc, &stmt);
    std::cout << "ret: " << ret << " SQL_SUCCEEDED(ret):" << SQL_SUCCEEDED(ret) << ", SQL_SUCCESS:" << SQL_SUCCESS << ", SQL_SUCCESS_WITH_INFO:" << SQL_SUCCESS_WITH_INFO << std::endl;
    if (!SQL_SUCCEEDED(ret))
    {
        SQLFreeConnect(dbc);

        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] SQLAllocStmt " << err_msg << ", " << err_num << std::endl;
        return 0;
    }
    SQLLEN numRecs = 0;
    SQLGetDiagField(SQL_HANDLE_ENV, env, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
    std::cout << "SQLGetDiagField" << numRecs << std::endl;
    SQLGetDiagField(SQL_HANDLE_DBC, dbc, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
    std::cout << "SQLGetDiagField" << numRecs << std::endl;
    SQLGetDiagField(SQL_HANDLE_STMT, stmt, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
    std::cout << "SQLGetDiagField" << numRecs << std::endl;



    // SQLGetStmtAttr(stmt, SQL_ATTR_QUERY_TIMEOUT, &get_ltimeout, sizeof(get_ltimeout), )

    // ============== connect end =================//

    // =============== check alive ==================//

    // ret = SQLExecDirect (stmt, (SQLCHAR *) "update tuto SET ID='1234d5' where name='sangho';", SQL_NTS);


    ret = SQLPrepare( stmt, (SQLCHAR*)"update tuto SET id=?, test=? where name=?;", SQL_NTS);
    if(ret != SQL_SUCCESS){
        std::cout << "SQLPrepare fail\n";
        return 0;
    }
    SQLINTEGER          sId = 543222;
    SQLCHAR             sTest[100] = "123";
    SQLCHAR             sName[100] = "sangho";


    ret = SQLBindParameter( stmt,
                            1,
                            SQL_PARAM_INPUT,
                            SQL_C_SLONG,
                            SQL_INTEGER,
                            0,
                            0,
                            &sId,
                            sizeof(sId),
                            NULL );
    if(ret != SQL_SUCCESS){
        std::cout << "SQLBindParameter 1 fail\n";
        return 0;
    }
    ret = SQLBindParameter( stmt,
                     2,
                     SQL_PARAM_INPUT,
                     SQL_C_CHAR,
                     SQL_CHAR,
                     0,
                     0,
                     sTest,
                     sizeof(sTest),
                     NULL );
    if(ret != SQL_SUCCESS){
        std::cout << "SQLBindParameter 2 fail\n";
        return 0;
    }                     
    ret = SQLBindParameter( stmt,
                     3,
                     SQL_PARAM_INPUT,
                     SQL_C_CHAR,
                     SQL_CHAR,
                     0,
                     0,
                     sName,
                     sizeof(sName),
                     NULL );
    if(ret != SQL_SUCCESS){
        std::cout << "SQLBindParameter 3 fail\n";
        return 0;
    }
    ret = SQLExecute( stmt );

    std::cout << "\n!!!SQLExecDirect ret: " << ret << std::endl;
    if( ret != SQL_SUCCESS)
    {

        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] SQLExecDirect " << err_msg << ", " << err_num << std::endl;
        if(err_num > 0){
            return 0;
        }
        // if(_err_num == 327730 || _err_num == 331843)
        // {
        //     SQLFreeStmt(_stmt, SQL_DROP);
        //     _stmt = SQL_NULL_HSTMT;
        // }

    }

    // ret = SQLMoreResults(stmt);

    
    SQLSMALLINT             now_num_of_columns = 0;
    ret = SQLNumResultCols (stmt, &now_num_of_columns);
    if(!SQL_SUCCEEDED(ret))
    {
        SQLFreeConnect(dbc);

        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] SQLNumResultCols " << err_msg << ", " << err_num << std::endl;
        return 0;
    }
    ret = SQLFetch(stmt);
    if(ret == SQL_NO_DATA){
        std::cout << "fetch no data" << std::endl;
        return 0;
    }

    std::cout << "now_num_of_columns: " << now_num_of_columns<< std::endl;

    SQLLEN indicator;
    char buffer[1024] = "";
    ret = SQLGetData(stmt, 1, SQL_C_CHAR, buffer, sizeof(buffer), &indicator);
    if(!SQL_SUCCEEDED(ret))
    {

        SQLINTEGER err_num = 0;
        SQLSMALLINT err_msg_len = 0;
        SQLCHAR err_msg[1024] = "";
        SQLError(env, dbc, stmt, NULL, &err_num, err_msg, sizeof(err_msg), &err_msg_len);
        std::cout << "[ERROR] SQLGetData " << err_msg << ", " << err_num << std::endl;

        SQLFreeConnect(dbc);
        return 0;
    }

    std::cout << "buffer:" << buffer << std::endl;

    ret = SQLFetch (stmt);
    if(SQL_NO_DATA == ret){
        std::cout << "SQL NO DATA" << std::endl;
    }
    
    ret = SQLFreeStmt(stmt, SQL_CLOSE);
    std::cout << "ret : " << ret << std::endl;

    ret = SQLDisconnect(dbc);
    std::cout << "ret : " << ret << std::endl;

    ret = SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    std::cout << "ret : " << ret << std::endl;






    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }



    // jdin::oop::db::st_result_t       _result; // ? 조회 결과 자료구조

    // SQLINTEGER                      _err_num;        //? 실패시 에러 값
    // SQLSMALLINT                     _err_msg_length;
    // SQLCHAR                         _err_message[1024];

    // std::string             _user;
    // std::string             _password;
    // std::string             _dsn;
    // long                    _connect_timeout;
    // long                    _login_timeout;
    // long                    _auto_commit;
    // SQLHDBC                 _dbc;
    // SQLHENV                 _env;
    // SQLHSTMT                _stmt;
    // jdin::oop::log::Logger  _logger;


}
