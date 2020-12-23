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
        .setConnectTimeout(10)
        .setLoginTimeout(10)
        .build();
    // auto conn = odbctemplate::OdbcConnect::get_connection("DSN=TST_DB;");
    {
        std::cout << "start test3 step 1\n";
        auto stmt = conn.allocStmt();
        auto fetcher = stmt.preparedExecute("update tuto set test=?", "a");
        auto result = fetcher.getRowCount();

        std::cout << result << std::endl;
    }
    {
        auto stmt = conn.allocStmt();
        auto preparedStmt = stmt.preparedStmt("update tuto set test=?");
        auto result1 = preparedStmt.bindExecute("a").getRowCount();
        std::cout << result1 << std::endl;
        auto result2 = preparedStmt.bindExecute("baa").getRowCount();
        std::cout << result2 << std::endl;
        auto result3 = preparedStmt.bindExecute("c").getRowCount();
        std::cout << result3 << std::endl;
    }
  
  {
        auto stmt = conn.allocStmt();
        
        auto preparedStmt = stmt.preparedStmt("update tuto set test=?");
        auto result1 = preparedStmt.bindExecute("a").getRowCount();
        std::cout << result1 << std::endl;
        auto result2 = conn.directExecute("update tuto set test='123123'").getRowCount();
        std::cout << result2 << std::endl;
        auto result3 = preparedStmt.Execute().getRowCount();
        std::cout << result3 << std::endl;
        


        

    }

    
    return 0;
}
