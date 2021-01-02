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
        
    auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=TST_DB;")
        .setAutocommit(true)
        .setConnectTimeout(10)
        .setLoginTimeout(10)
        .build();
    // auto conn = odbctemplate::OdbcConnect::get_connection("DSN=TST_DB;");
    {
        std::cout << "start test3 step 1\n";
        int id = 12;
        char buf1[32] = "testname";
        char buf2[32] = "test";
        char buf3[32] = "testaddress";
        auto preparedStmt = conn.preparedStmt("INSERT INTO tuto (id, name, test, address) VALUES (?, ?, ?, ?)");
        auto result = preparedStmt.bindExecute(id, buf1, buf2, buf3).getInsertRowCount();
        std::cout << result << std::endl;
    }

    
    return 0;
}
