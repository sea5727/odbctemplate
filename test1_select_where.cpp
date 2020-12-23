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
        .build();

    {
        auto result1= conn.allocStmt()
            .directExecute("select 1;")
            .fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                int result;
                result = helper.getLong();
                return result;
            });
        
        std::cout << "result1:" << result1.size() << std::endl;
        for(auto & result : result1){
            std::cout << result << std::endl;
        }
    }


    {
        auto result2 = 
            conn.preparedExecute("select id, test, name from tuto where name=?;", "sanghoasdfasdf")
            .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                tuto result;
                result.id = helper.getLong();
                result.test = helper.getString();
                result.name = helper.getString();
                return result;
            });
        std::cout << "result2 : " << std::endl;
        for(auto & tuto : result2){
            tuto.print();
        }
    }
    {
        auto preparedStmt = conn.preparedStmt("select id, test, name from tuto where name=?;");
        auto resultlist =  preparedStmt.bindExecute("sangho")
            .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                tuto result;
                result.id = helper.getLong();
                result.test = helper.getString();
                result.name = helper.getString();
                return result;
            });
        std::cout << "resultlist : " << std::endl;
        for(auto & tuto : resultlist){
            tuto.print();
        }
        auto resultlist2 =  preparedStmt.bindExecute("sangho2")
            .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                tuto result;
                result.id = helper.getLong();
                result.test = helper.getString();
                result.name = helper.getString();
                return result;
            });
        std::cout << "resultlist : " << std::endl;
        for(auto & tuto : resultlist2){
            tuto.print();
        }
        auto resultlist3 =  preparedStmt.bindExecute("sanghodasdas")
            .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                tuto result;
                result.id = helper.getLong();
                result.test = helper.getString();
                result.name = helper.getString();
                return result;
            });
        std::cout << "resultlist : " << std::endl;
        for(auto & tuto : resultlist3){
            tuto.print();
        }
    }

}
