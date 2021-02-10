# odbctemplate

Easy db library for C++ wrapping the ODBC client C library

ODBC C 라이브러리를 C++로 랩핑한 쉬운 DB 라이브러리


install : yum install unixODBC-devel

maria-db install : https://blog.sleeplessbeastie.eu/2018/01/08/how-to-install-and-configure-mariadb-unixodbc-driver/

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

Simple Connection Pool 
Simple BenchMark
   

### Example : Connection 

```cpp
#include <odbctemplate/odbctemplate.h>

auto conn = odbctemplate::OdbcConnectBuilder()
    .setDsn("DSN=TST_DB;")
    .setAutocommit(false)
    .setLoginTimeout(100)
    .build();

```


### Example : SELECT 

```cpp

class tuto {
public:
    int id;
    std::string name;
    std::string test;
};


auto result = 
    conn.preparedExecute("select id, test, name from tuto where name=?;", "searchname")
    .fetch<tuto>([](odbctemplate::FetchHelper helper){
        tuto result;
        result.id = helper.getLong();
        result.test = helper.getString();
        result.name = helper.getString();
        return result;
    });
for(auto & tuto : result){
    tuto.print();
}

```

### Example : UPDATE / INSERT / DELETE 

```cpp

auto succ = conn.preparedExecute("UPDATE tuto SET test=? where name = ?", "teatvalue", "searchname")
    .getUpdateRowCount();

std::cout << succ << std::endl;

```


### Example : REUSE QUERY ( OdbcPreparedStmt )

```cpp

auto preparedStmt = conn.preparedStmt("select id, test, name from tuto where id=?;");

int i = 0;

while(1){
    auto result =  preparedStmt.bindExecute(i) //  Execute with parameter binding
        .fetch<tuto>([](odbctemplate::FetchHelper helper){
            tuto result;
            result.id = helper.getLong();
            result.test = helper.getString();
            result.name = helper.getString();
            return result;
        });
    i += 1;
    std::this_thread::sleep_for(std::chrono::seconds(1))
}

```


### Example : SELECT LARGE AMOUNT OF DATA ( BindColStmt / BindColHelper )

```cpp

class tuto2 {
public:
    long id;
    char name[64 + 1];
    char test[64 + 1];
    char address[128 + 1];
    void 
    print(){
        std::cout << "id : " << id << std::endl;
        std::cout << "name : " << name << std::endl;
        std::cout << "test : " << test << std::endl;
        std::cout << "address : " << address << std::endl;
    }
};

... 

tuto2 result;

auto preparedStmt = conn.preparedStmt("select id, name, test, address from tuto")
    .bindCol([&](odbctemplate::BindColHelper helper){
        helper.setBindColLong(&result.id);
        helper.setBindColString(result.name, sizeof(result.name));
        helper.setBindColString(result.test, sizeof(result.test));
        helper.setBindColString(result.address, sizeof(result.address));
    });

while(1){
    auto fetcher = preparedStmt.Execute();
    auto sel_count = 0;
    while(fetcher.fetch()){
        sel_count++;
        result.print();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

while(fetcher.fetch()){
    // result.print();
}

```




