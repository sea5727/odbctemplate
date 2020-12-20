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
    auto stmt = conn.execute();

    stmt.queryForObject("update tuto set test='new_test' where name=?;", "sangho")
        .fetch();

    getchar();

    stmt.queryForObject("update tuto set test='haha' where name=?;", "sangho");

    
    return 0;
}
