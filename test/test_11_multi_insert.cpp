#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate.hpp"


class TSMS_HISTORY{
public:
    odbctemplate::Int64 MSG_SEQ;
    odbctemplate::NullInt64 PROC_RESULT;
    odbctemplate::NullInt64 IN_SECT;
    odbctemplate::Char<16 + 1> SC_TIME;
    odbctemplate::NullChar<16 + 1> SEND_TIME;
    odbctemplate::NullChar<16 + 1> IN_SIP_URI;
    
    void
    print(){
        printf("MSG_SEQ:%d, PROC_RESULT(%d:%d), IN_SECT(%d:%d), SC_TIME:%s, SEND_TIME(%d:%s), IN_SIP_URI(%d:%s)\n", 
            MSG_SEQ.Value,
            PROC_RESULT.Valid(), PROC_RESULT.Valid() == true ? PROC_RESULT.Value : 0,
            IN_SECT.Valid(), IN_SECT.Valid() == true ? IN_SECT.Value : 0,
            SC_TIME.Value,
            SEND_TIME.Valid(), SEND_TIME.Valid() == true ? SEND_TIME.Value: "", 
            IN_SIP_URI.Valid(), IN_SIP_URI.Valid() == true ? IN_SIP_URI.Value: "");
    }
};


int main(int argc, char* argv[]) {
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(true)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();
    
    
    TSMS_HISTORY result;

    const char * sql = "INSERT INTO tuto (name, test, address) VALUES(?, ?, ?);";

    auto dbc = conn.dbc;
    SQLRETURN status = 0;
    SQLROWSETSIZE row_count;
    SQLUSMALLINT row_status[ 20 ];
    SQLRETURN ret;

    SQLUSMALLINT ParamStatusArray[20];

    /* variables to contain returned data */
    SQLCHAR table[ 20 ][ 31 ];
    SQLCHAR column[ 20 ][ 31 ];
    SQLINTEGER type[ 20 ];
    SQLLEN indicator1[ 20 ], indicator2[ 20 ], indicator3[ 20 ];

    SQLINTEGER id[20];
    SQLCHAR name[20][64 + 1];
    SQLCHAR test[20][64 + 1];
    SQLCHAR address[20][128 + 1];

    SQLHDBC stmt = SQL_NULL_HSTMT;

    status = SQLAllocHandle(SQL_HANDLE_STMT, dbc->dbc, &stmt);
    if( status != SQL_SUCCESS){
        printf("status:%d (%s/%d)\n", status, __func__, __LINE__);
        return 0;
    }

    
    status = SQLSetStmtAttr (stmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER) 20, 0);
    if( status != SQL_SUCCESS){
        printf("status:%d (%s/%d)\n", status, __func__, __LINE__);
        return 0;
    }
    status = SQLSetStmtAttr (stmt, SQL_ATTR_PARAM_STATUS_PTR, ParamStatusArray, 20);
    if( status != SQL_SUCCESS){
        printf("status:%d (%s/%d)\n", status, __func__, __LINE__);
        return 0;
    }
    for(int i = 0 ; i < 20 ; i++){
        strcpy((char *)name[i], "namefirst");
        strcpy((char *)test[i], "testfirst");
        strcpy((char *)address[i], "addressfirst");
    }

    // Bind array values of parameters 1-4
    status = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, name, sizeof(name[0]), NULL);
    if( status != SQL_SUCCESS){
        printf("status:%d (%s/%d)\n", status, __func__, __LINE__);
        return 0;
    }
    status = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, test, sizeof(test[0]), NULL);
    if( status != SQL_SUCCESS){
        printf("status:%d (%s/%d)\n", status, __func__, __LINE__);
        return 0;
    }
    status = SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, address, sizeof(address[0]), NULL);
    if( status != SQL_SUCCESS){
        printf("status:%d (%s/%d)\n", status, __func__, __LINE__);
        return 0;
    }

    status = SQLExecDirect(stmt, (SQLCHAR *)sql, SQL_NTS);
    if( status != SQL_SUCCESS){
        printf("status:%d (%s/%d)\n", status, __func__, __LINE__);
        return 0;
    }

	for (int i = 0; i < 20; i++) {
		printf(" Parameter Set ");
		switch (ParamStatusArray[i]) {
			case SQL_PARAM_SUCCESS:
			case SQL_PARAM_SUCCESS_WITH_INFO:
				printf("%d Success\n", i);
				break;
			case SQL_PARAM_ERROR:
				printf("%d Error\n", i);
				break;
			case SQL_PARAM_UNUSED:
				printf("%d Not processed\n", i);
				break;
			case SQL_PARAM_DIAG_UNAVAILABLE:
				printf("%d Status Unknown\n", i);
				break;
		}
	}

    return 0;

}

