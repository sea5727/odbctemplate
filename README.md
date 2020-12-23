# odbctemplate

Easy library for C++ wrapping the ODBC client C library

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


### Example : REUSE QUERY

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



