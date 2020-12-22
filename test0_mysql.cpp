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

    {
        auto stmt = conn.allocStmt();
        auto prepareStmt = stmt.prepareStmt("select ?;");
        auto fetcher = prepareStmt.bindExecute(1);
        auto result = fetcher.fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
                int result;
                result = helper.getLong();
                return result;
            });
        std::cout << result.at(0) << std::endl;
    }
    
    // std::cout << "=============== 1 ======================\n";
    // auto results 
    //     = conn.preparedExecute("select ?", 1)
    //         .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });

    // std::cout << "=============== 2 ======================\n";
    // auto pre = conn.preparedStmt("select ?");
    // auto results1 = pre.bindExecute(1)
    //     .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //         int result;
    //         result = helper.getLong();
    //         return result;
    //     });
        

    // std::cout << "=============== 3 ======================\n";
    // auto results2 = pre.bindExecute(10)
    //     .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //         int result;
    //         result = helper.getLong();
    //         return result;
    //     });

    
    // std::cout << "r:" << results.at(0) << std::endl;
    // std::cout << "r1:" << results1.at(0) << std::endl;
    // std::cout << "r2:" << results2.at(0) << std::endl;

}
