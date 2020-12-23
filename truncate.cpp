#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>
#include <fstream>

#include "odbctemplate/odbctemplate.hpp"



class myclass{
public:
    myclass()
        : a{0}
        , b{""} {
        std::cout << "my class default create\n";
    }
    myclass(int a, std::string b)
        : a{a}
        , b{b} {
        std::cout << "my class default create\n";
    }
    myclass(const myclass & copy)
        : a{copy.a}
        , b{copy.b} {
        std::cout << "my class copy create\n";
    }
    myclass(myclass && move)
        : a{move.a}
        , b{std::move(move.b)} {
        std::cout << "my class move create\n";
    }
    ~myclass(){
        std::cout << "my class delete\n";
    }
    void print(){
        std::cout << "a : " << a << std::endl;
        std::cout << "b : " << b << std::endl;
    }
    int a;
    std::string b;
};

class container{
public:
    std::shared_ptr<myclass> value;
    container() 
        : value{nullptr} {
        std::cout << "container default create\n";
    }
    container(std::shared_ptr<myclass> value) 
        : value{value} {
        std::cout << "container shared_ptr create\n";
    }
    ~container(){
        std::cout << "container delete\n";
    }
};


const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

void 
truncate(
    odbctemplate::OdbcConnect & conn, const std::string & table){
    
    std::string select_query = "select count(*) from " + table;
    std::string truncate_query = "TRUNCATE TABLE " + table;
    auto count = conn.directExecute(select_query)
        .fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
            int result;
            result = helper.getLong();
            return result;
        });
    if(count.size() > 0){
        std::cout << table << "select count : " << count.at(0) << std::endl;
        if(count.at(0) > 1000){
            auto success = conn.directExecute(truncate_query)
                .getRowCount();
            std::cout << table << "truncate count : " << success << std::endl;
        }
    }
}

int main(int argc, char * argv[]) {


    auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=RCS_DSN_NEW;UID=rcs;PWD=rcs.123;")
        .setAutocommit(true)
        .setLoginTimeout(10)
        .build();


    auto now = std::chrono::system_clock::now();
    auto p = std::chrono::system_clock::time_point(now);
    auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);
    auto next_time = start_time + std::chrono::seconds(1);
    std::this_thread::sleep_until(next_time);

    
    while(1){

        
        auto now = std::chrono::system_clock::now();

        std::cout << "[START] ===== " << currentDateTime() << std::endl; 
        auto p = std::chrono::system_clock::time_point(now);
        auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);

        auto next_time = start_time + std::chrono::seconds(10);

        truncate(conn, "TBL_HIST_SMS");
        truncate(conn, "TBL_HIST_LMS");
        truncate(conn, "TBL_HIST_MMS");

        std::this_thread::sleep_until(next_time);
    }

}  

