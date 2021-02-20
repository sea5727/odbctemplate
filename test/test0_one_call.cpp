#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate.hpp"


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
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setDsn("DSN=TST_DB;")
        .setAutocommit(true)
        .setLoginTimeout(10)
        .build();

    // {
    //     auto prepareStmt = conn.prepareStmt("select ? from TBL_SEND_SMS;"); // ! 저장 필요
    //     auto fetcher = prepareStmt.bindExecute(1);
    //     auto result = fetcher.fetch<int>([](odbctemplate::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });

    
    //     std::cout << "r:" << result.at(0) << std::endl;
    // }
    // {
    //     auto prepareStmt = conn.prepareStmt("select ? from TBL_SEND_SMS;"); // ! 저장 필요
    //     auto fetcher = prepareStmt.bindExecute(1);
    //     auto result = fetcher.fetch<int>([](odbctemplate::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });


    //     std::cout << "r:" << result.at(0) << std::endl;

    // }
    // {
    //     auto fetcher = conn.preparedExecute("select ? from TBL_SEND_SMS;", 1);
    //     auto result = fetcher.fetch<int>([](odbctemplate::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });

    
    //     std::cout << "r:" << result.at(0) << std::endl;

    // }

    // {
    //     auto result = conn.preparedExecute("select ? from TBL_SEND_SMS;", 1)
    //         .fetch<int>([](odbctemplate::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });
    
    //     std::cout << "r:" << result.at(0) << std::endl;
    // }

    // {
    //     auto result = conn.preparedExecute("select 1 from TBL_SEND_SMS;")
    //         .fetch<int>([](odbctemplate::FetchHelper helper){
    //             int result;
    //             result = helper.getLong();
    //             return result;
    //         });
    
    //     std::cout << "r:" << result.at(0) << std::endl;

    // }

    {
        auto result = conn.directExecute("select 1 from TBL_SEND_SMS;")
                .fetch<int>([](odbctemplate::FetchHelper helper){
            int result;
            result = helper.getLong();
            return result;
        });
    
        std::cout << "r:" << result.at(0) << std::endl;

    }

}
