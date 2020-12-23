# odbctemplate

Easy db library for C++ wrapping the ODBC client C library

ODBC C 라이브러리를 C++로 랩핑한 쉬운 DB 라이브러리

### Example : Connection 

```cpp
#include <odbctemplate/odbctemplate.h>

auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=TST_DB;")
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
    .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
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
        .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
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
    .bindCol([&](odbctemplate::OdbcpreparedStmt::BindColHelper helper){
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




