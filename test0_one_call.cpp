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
    auto conn = odbctemplate::OdbcConnect::get_connection("DSN=RCS_DSN_NEW;UID=rcs;PWD=rcs.123;");

    // {
    //     auto stmt = conn.allocStmt();
    //     auto prepareStmt = stmt.prepareStmt("select ? from TBL_SEND_SMS;"); // ! 저장 필요
    //     auto fetcher = prepareStmt.bindExecute(1);
    //     auto result = fetcher.fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });


    //     stmt.print_mystmt();
    //     prepareStmt.print_mystmt();
    //     fetcher.print_mystmt();
    
    //     std::cout << "r:" << result.at(0) << std::endl;
    // }
    // {
    //     auto prepareStmt = conn.preparedStmt("select ? from TBL_SEND_SMS;"); // ! 저장 필요
    //     auto fetcher = prepareStmt.bindExecute(1);
    //     auto result = fetcher.fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });


    //     prepareStmt.print_mystmt();
    //     fetcher.print_mystmt();
    
    //     std::cout << "r:" << result.at(0) << std::endl;

    // }
    // {
    //     auto fetcher = conn.preparedExecute("select ? from TBL_SEND_SMS;", 1);
    //     auto result = fetcher.fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });

    //     fetcher.print_mystmt();
    
    //     std::cout << "r:" << result.at(0) << std::endl;

    // }

    // {
    //     auto result = conn.preparedExecute("select ? from TBL_SEND_SMS;", 1)
    //         .fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });
    
    //     std::cout << "r:" << result.at(0) << std::endl;
    // }

    // {
    //     auto result = conn.preparedExecute("select 1 from TBL_SEND_SMS;")
    //         .fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });
    
    //     std::cout << "r:" << result.at(0) << std::endl;

    // }

    {
        auto result = conn.directExecute("select 1 from TBL_SEND_SMS;")
                .fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
            int result;
            result = helper.getLong();
            return result;
        });
    
        std::cout << "r:" << result.at(0) << std::endl;

    }

}
