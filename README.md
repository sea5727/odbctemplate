# odbctemplate

db library wrapped ODBC

example 
1.  SELECT
-  auto result = conn.preparedExecute("select id, test, name from tuto where name=?;", "searchname")
            .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                tuto result;
                result.id = helper.getLong();
                result.test = helper.getString();
                result.name = helper.getString();
                return result;
            });
    for(auto & tuto : result2){
        tuto.print();
# Example ( Connection )

```cpp

auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=TST_DB;")
    .setAutocommit(false)
    .setLoginTimeout(100)
    .build();

```


# Example ( SELECT )

```cpp
#include <odbctemplate/odbctemplate.h>


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

# Example ( UPDATE / INSERT / DELETE )

```cpp

auto succ = conn.preparedExecute("UPDATE tuto SET test=? where name = ?", "teatvalue", "searchname")
    .getUpdateRowCount();

std::cout << succ << std::endl;

```
