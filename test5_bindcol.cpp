#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate/odbctemplate.hpp"


class tuto {
public:
    int id;
    std::string name;
    std::string test;

    void print(){
        std::cout << "id:" << id << ",name:" << name << ",test:" << test<< std::endl;
    }
};


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

int main(int, char**) {
        
    auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=TST_DB;")
        .setAutocommit(true)
        .setLoginTimeout(10)
        .build();

    tuto2 result;

    char buffer[1024] = "";
    auto fetcher = conn.preparedStmt("select id, name, test, address from tuto")
        .bindCol([&](odbctemplate::OdbcpreparedStmt::BindColHelper helper){
            helper.setBindColLong(&result.id);
            helper.setBindColString(result.name, sizeof(result.name));
            helper.setBindColString(result.test, sizeof(result.test));
            helper.setBindColString(result.address, sizeof(result.address));
        }).Execute();
    
    while(fetcher.fetch()){
        // result.print();
    }

    return 0;
}
