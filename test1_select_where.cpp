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
    auto conn = odbctemplate::OdbcConnect::get_connection("DSN=TST_DB;");

    auto container2 = conn.execute()
        .queryForObject("select 1;")
        .fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
            int result;
            result = helper.getLong();
            return result;
        });
    
    std::cout << "select 1 success\n";
    for(auto & tuto : container2){
        std::cout << tuto << std::endl;
    }
    std::cout << "select 1 end\n";



    auto container = conn.execute()
        .queryForObject("select id, test, name from tuto where name=?;", "sangho")
        .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
            tuto result;
            result.id = helper.getLong();
            result.test = helper.getString();
            result.name = helper.getString();
            result.print();
            return result;
        });

    std::cout << "select success\n";
    for(auto & tuto : container){
        tuto.print();
    }
    std::cout << "select end\n";




}
