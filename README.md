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
    -   Int16
    -   NullInt16
    -   Float
    -   NullFloat
    -   String
    -   NullString
    -   Bool
    -   NullBool


# Example 
## data Type example
```cpp
odbctemplate::NullChar<10> COLUMN1;
odbctemplate::NullInt64 COLUMN2;
odbctemplate::NullInt16 COLUMN3;
odbctemplate::NullInt16 COLUMN3;
odbctemplate::Bool COLUMN4;
odbctemplate::Float64 COLUMN5;

```
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
즉시 Insert/Update/Delete 쿼리를 사용하는 예제
```cpp
    auto count = 
    conn.directExecute("UPDATE TSMS_HISTORY SET SC_TIME=20210302 where MSG_SEQ=10590")
        .getUpdateRowCount();
```
-   SELECT   
Select 예제
```cpp
TSMS_HISTORY data;
auto fetcher = 
    conn.directExecute("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY;")
    .bindCol(1, &data.MSG_SEQ)
    .bindCol(2, &data.PROC_RESULT)
    .bindCol(3, &data.IN_SECT)
    .bindCol(4, &data.SC_TIME)
    .bindCol(5, &data.SEND_TIME)
    .bindCol(6, &data.IN_SIP_URI);

while(fetcher.fetch()){
    std::cout << "MSG_SEQ: " << data.MSG_SEQ << std::endl;
    std::cout << "PROC_RESULT: " << data.PROC_RESULT << std::endl;
    std::cout << "IN_SECT: " << data.IN_SECT << std::endl;
    std::cout << "SC_TIME: " << data.SC_TIME << std::endl;
    std::cout << "SEND_TIME: " << data.SEND_TIME << std::endl;
    std::cout << "IN_SIP_URI: " << data.IN_SIP_URI << std::endl;
}

```

## preparedExecute ( Binding Query )   
binding query example : generic type
```cpp
auto fetcher = 
conn.preparedExecute("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ > ? and MSG_SEQ < ?;", 10587, 10631)
    .bindCol(1, &data.MSG_SEQ)
    .bindCol(2, &data.PROC_RESULT)
    .bindCol(3, &data.IN_SECT)
    .bindCol(4, &data.SC_TIME)
    .bindCol(5, &data.SEND_TIME)
    .bindCol(6, &data.IN_SIP_URI);

while(fetcher.fetch()){
    //TODO 
}

```

## Example : preparedStmt ( REUSE QUERY )

```cpp
// reuse stmt handler
auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ = ?;");

for(int i = 10590 ; i < 10631; i++){
    auto fetcher = stmt.bindExecute(i)
        .bindCol(1, &data.MSG_SEQ)
        .bindCol(2, &data.PROC_RESULT)
        .bindCol(3, &data.IN_SECT)
        .bindCol(4, &data.SC_TIME)
        .bindCol(5, &data.SEND_TIME)
        .bindCol(6, &data.IN_SIP_URI);
    if(fetcher.fetch()){
        //TODO
    }
}
```

## Example : UPDATE / INSERT / DELETE (Reuse)

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


