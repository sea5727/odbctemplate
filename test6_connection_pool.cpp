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
    std::cout << "start\n";
    auto db_connection_pool = odbctemplate::OdbcConnectionPool::get_connection_pool("DSN=TST_DB;", 4, 32);
    std::cout << "get_connection_idx\n";

    auto & pool_1 = db_connection_pool.get_connection_idx(1);
    auto & pool_10 = db_connection_pool.get_connection_idx(10);


    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
