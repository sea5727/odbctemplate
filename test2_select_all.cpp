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

    odbctemplate::OdbcManager & pm = odbctemplate::OdbcManager::getInstance();

    auto conn = odbctemplate::OdbcConnect::get_connection("DSN=TST_DB;");
    auto container = conn.execute()
        .queryForObject("select id, test, name from tuto;")
        .fetch<tuto>([](odbctemplate::Fetcher::FetchHelper helper){
            tuto result;
            result.id = helper.getLong();
            result.test = helper.getString();
            result.name = helper.getString();
            result.print();
            return result;
    }, 150);

    std::cout << "select end.. print!!\n";
    for(auto & tuto : container){
        tuto.print();
    }

    

    return 0;
}
