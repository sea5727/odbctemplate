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
    auto conn = odbctemplate::OdbcConnect::get_connection("DSN=RCS_DSN_NEW;UID=rcs;PWD=rcs.123");
    const char * query = "select 1 from DUAL;";

    // auto resetedStmt = conn.allocStmt();

    // auto results 
    //     = conn.preparedExecute("select ? from TBL_SEND_SMS;", 1)
    //         .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });

    // auto pre = conn.preparedStmt("selet ? from TBL_SEND_SMS;");
    // auto results1 = pre.bindExecute(1)
    //     .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //         int result;
    //         result = helper.getLong();
    //         return result;
    //     });
        
    // auto results2 = pre.bindExecute(10)
    //     .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //         int result;
    //         result = helper.getLong();
    //         return result;
    //     });

    // auto results = conn.allocStmt().queryForObject(query)
    //     .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //         int result;
    //         result = helper.getLong();
    //         return result;
    //     });
    std::cout << "r1:" << results1.at(0) << std::endl;
    std::cout << "r2:" << results2.at(0) << std::endl;

    getchar();
    return 0;



    // auto container2 = conn.execute()
    //     .queryForObject("select 1;")
    //     .fetch<int>([](odbctemplate::Fetcher::FetchHelper helper){
    //         int result;
    //         result = helper.getLong();
    //         return result;
    //     });
    
    // std::cout << "select 1 success\n";
    // for(auto & tuto : container2){
    //     std::cout << tuto << std::endl;
    // }
    // std::cout << "select 1 end\n";



    // auto container = conn.execute()
    //     .queryForObject("select id, test, name from tuto where name=?;", "sangho")
    //     .fetch<tuto>([](odbctemplate::Fetcher::FetchHelper helper){
    //         tuto result;
    //         result.id = helper.getLong();
    //         result.test = helper.getString();
    //         result.name = helper.getString();
    //         result.print();
    //         return result;
    //     });

    // std::cout << "select success\n";
    // for(auto & tuto : container){
    //     tuto.print();
    // }
    // std::cout << "select end\n";




}
