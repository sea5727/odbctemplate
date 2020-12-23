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

int main(int, char**) {

    auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=TST_DB;")
        .setAutocommit(true)
        .setLoginTimeout(10)
        .build();

    auto result1 = conn.directExecute("select id, test, name from tuto;")
            .fetch<tuto, 1>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                tuto result;
                result.id = helper.getLong();
                result.test = helper.getString();
                result.name = helper.getString();
                return result;
        });


    std::cout << "select end.. print!!\n";
    for(auto & tuto : result1){
        tuto.print();
    }


    return 0;
}
