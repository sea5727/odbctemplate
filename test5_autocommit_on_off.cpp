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

    auto conn = odbctemplate::OdbcConnect::get_connection("DSN=TST_DB;", 0);
    auto stmt = conn.execute();

    stmt.queryForObject("update tuto set test=? where name=?;", "54321", "sangho3")
        .fetch(SQL_RESET_PARAMS); // reuse query

    getchar();

    stmt.requeryForObject("com_test", "sangho");

    conn.commit();


    
    return 0;
}
