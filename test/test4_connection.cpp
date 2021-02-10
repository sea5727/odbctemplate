#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate.hpp"


class tuto {
public:
    int id;
    std::string name;
    std::string test;

    void print(){
        std::cout << "id:" << id << ",name:" << name << ",test:" << test<< std::endl;
    }
};

int main(int, char**) {
        
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setDsn("DSN=TST_DB;")
        .setAutocommit(false)
        .setLoginTimeout(100)
        .build();

    auto prev_autocommit = odbctemplate::OdbcConnectBuilder(conn)
        .getAutocommit();

    std::cout << "prev autocommit : " << prev_autocommit << std::endl;

    odbctemplate::OdbcConnectBuilder(conn)
        .setAutocommit(true);

    auto after_autocommit = odbctemplate::OdbcConnectBuilder(conn)
        .getAutocommit();
    std::cout << "current after_autocommit : " << after_autocommit << std::endl;
    auto dead = odbctemplate::OdbcConnectBuilder(conn)
        .getConnectionDead();
    std::cout << "current dead : " << dead << std::endl;
    
    auto reconn = odbctemplate::OdbcConnectBuilder(conn)
        .build();
    // std::cout << "current loginTimeout2 : " << loginTimeout2 << std::endl;

    return 0;
}
