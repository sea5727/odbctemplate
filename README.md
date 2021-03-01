# odbctemplate


Easy db library for C++ wrapping the ODBC client C library

ODBC C 라이브러리를 C++로 랩핑한 쉬운 DB 라이브러리

header-only like crow


# ODBC ? 

Open Database Connectivity : 어떤 데이터베이스라도 접근할 수 있게 만든 마이크로소픝트에서 만든 표준 방법.

HENV : ODBC 버전 및 환경 HANDLE , HENV(1) : HDBC(N) 의 관계

HDBC : DB 커넥션 HANDLE, HDBC(1) : HSTMT(N) 의 관계

HSTMT : DB 쿼리 HANDLE.


# SOME ODBC API

SQLAllocHandle : ODBC 핸들 할당   
SQLExecDirect : 쿼리문 하나를 전송   
SQLPrepare : 쿼리문 하나를 Stmt 에 저장   
SQLBindParameter : Prepare 쿼리문에 "?" 에 변수를 바인드   
SQLBindCol : Select 결과 변수에 메모리 바인드   
SQLExecute : Prepare 한 쿼리문을 전송   
SQLFetch : Select 문을 Excute/ExecDirect 후 결과값 (1 row) 을 수신 [ BindCol로 매핑하였으면 해당 메모리에, 안하였으면 SQLGetData 로 하나씩 호출 ]   
SQLGetData : Select 문 결과를 하나씩 호출   
SQLNumREsultCols : Select 결과의 Column 호출   
SQLRowCount : Update/Insert/Delete 결과 Row 호출   

# TODO LIST

커넥션풀 만들어야 하나?


# Example : Connection 

```cpp
#include <odbctemplate/odbctemplate.hpp>

auto conn = odbctemplate::OdbcConnectBuilder()
    .setAutocommit(true)
    .setLoginTimeout(30)
    .setDsn("DSN=TST_DB;")
    .build();
```
# Data Type
-   Type들.
    -   Int64
    -   NullInt64
    -   Int32
    -   NullInt32
    -   Float
    -   NullFloat
    -   String
    -   NullString
    -   Bool
    -   NullBool


# Example 
## Data Object
```cpp
class TSMS_HISTORY{
public:
    odbctemplate::Int64 MSG_SEQ;
    odbctemplate::NullInt64 PROC_RESULT;
    odbctemplate::NullInt64 IN_SECT;
    odbctemplate::String SC_TIME;
    odbctemplate::NullString SEND_TIME;
    odbctemplate::NullString IN_SIP_URI;
};
```
## directExecute

-   INSERT/UPDATE/DELETE
```cpp
    auto count = 
    conn.directExecute("UPDATE TSMS_HISTORY SET SC_TIME=20210302 where MSG_SEQ=10590")
        .getUpdateRowCount();
```
-   SELECT
```cpp
auto ret = 
    conn.directExecute("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY;")
    .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
        TSMS_HISTORY ret;
        ret.MSG_SEQ = helper.getInt64();
        ret.PROC_RESULT = helper.getNullInt64();
        ret.IN_SECT = helper.getNullInt64();
        ret.SC_TIME = helper.getString();
        ret.SEND_TIME = helper.getNullString();
        ret.IN_SIP_URI = helper.getNullString();
        return ret;
    });

for(auto & result : ret){
    result.print();
}

```

## preparedExecute ( Binding Query )
```cpp
auto ret = 
conn.preparedExecute("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ > ? and MSG_SEQ < ?;", 10587, 10631)
    .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
    TSMS_HISTORY ret;
    ret.MSG_SEQ = helper.getInt64();
    ret.PROC_RESULT = helper.getNullInt64();
    ret.IN_SECT = helper.getNullInt64();
    ret.SC_TIME = helper.getString();
    ret.SEND_TIME = helper.getNullString();
    ret.IN_SIP_URI = helper.getNullString();
    return ret;
});

printf("size:%d\n", ret.size());
for(auto & result : ret){
    result.print();
}
```

## Example : preparedStmt ( REUSE QUERY )

```cpp
// reuse stmt handler
auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ = ?;");

for(int i = 10590 ; i < 10631; i++){
    auto ret = stmt.bindExecute(i)
        .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
            TSMS_HISTORY ret;
            ret.MSG_SEQ = helper.getInt64();
            ret.PROC_RESULT = helper.getNullInt64();
            ret.IN_SECT = helper.getNullInt64();
            ret.SC_TIME = helper.getString();
            ret.SEND_TIME = helper.getNullString();
            ret.IN_SIP_URI = helper.getNullString();
            return ret;
        });
    printf("size:%d\n", ret.size());
    for(auto & result : ret){
        result.print();
    }
}
```

## Example : UPDATE / INSERT / DELETE 

```cpp
// update one line 
auto count = 
conn.preparedExecute("UPDATE TSMS_HISTORY SET SC_TIME=? where MSG_SEQ=?", fmt, 10590)
    .getUpdateRowCount();
printf("count:%d\n", count);

// reuse update handler
auto stmt = conn.preparedStmt("UPDATE TSMS_HISTORY SET SC_TIME=? where MSG_SEQ=?")
for(int i = 10591 ; i < 10595 ; i++){
    auto cnt = stmt.bindExecute(fmt, i).getRowCount();
    printf("count:%d\n", cnt);
}

```





## Example : BindCol ( Select Many )

```cpp
TSMS_HISTORY result;

auto stmt = 
conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY;")
    .bindResultCol([&result](odbctemplate::BindColHelper & helper){
        helper.setInt64NotNull(&result.MSG_SEQ);
        helper.setInt64Nullable(&result.PROC_RESULT);
        helper.setInt64Nullable(&result.IN_SECT);
        helper.setCharNotNull(&result.SC_TIME);
        helper.setCharNullable(&result.SEND_TIME);
        helper.setCharNullable(&result.IN_SIP_URI);
    });

auto fetcher = stmt.Execute();
while(fetcher.fetch()){
    result.print();
}

```




