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

    {
        auto prepareStmt = conn.preparedStmt("select ? from TBL_SEND_SMS;"); // ! 저장 필요
        auto fetcher = prepareStmt.bindExecute(1);
        auto result = fetcher.fetch<int>([](odbctemplate::FetchHelper helper){
                int result;
                result = helper.getLong();
                return result;
            });


    
        std::cout << "r:" << result.at(0) << std::endl;


        auto prepareStmt2 = conn.preparedStmt("select ? from TBL_SEND_FILE;"); // ! 저장 필요;
        auto fetcher2 = prepareStmt.bindExecute(10);
        auto result2 = fetcher2.fetch<int>([](odbctemplate::FetchHelper helper){
                int result;
                result = helper.getLong();
                return result;
            });

        // auto fetcher4 = prepareStmt.bindExecute(3);

        // auto result2 = fetcher2.fetch<int>([](odbctemplate::FetchHelper helper){
        //         int result;
        //         result = helper.getLong();
        //         return result;
        //     });
        // auto result3 = fetcher3.fetch<int>([](odbctemplate::FetchHelper helper){
        //         int result;
        //         result = helper.getLong();
        //         return result;
        //     });

        // auto result4 = fetcher4.fetch<int>([](odbctemplate::FetchHelper helper){
        //         int result;
        //         result = helper.getLong();
        //         return result;
        //     });

    
        std::cout << "Result : " << std::endl;
        std::cout << result.size() << std::endl;
        std::cout << result2.size() << std::endl;
        // std::cout << result2.size() << std::endl;
        // std::cout << result3.size() << std::endl;
        // std::cout << result4.size() << std::endl;
    }
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

    // {
    //     auto result = conn.directExecute("select 1 from TBL_SEND_SMS;")
    //             .fetch<int>([](odbctemplate::FetchHelper helper){
    //         int result;
    //         result = helper.getLong();
    //         return result;
    //     });
    
    //     std::cout << "r:" << result.at(0) << std::endl;

    // }

}