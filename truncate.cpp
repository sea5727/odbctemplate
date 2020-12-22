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


int main(int argc, char * argv[]) {


    auto conn = odbctemplate::OdbcConnect::get_connection("DSN=RCS_DSN_NEW;UID=rcs;PWD=rcs.123;");


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

        {
            std::string query = "select count(*) from TBL_HIST_SMS";
            auto count = conn.directExecute(query)
                .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                    int result;
                    result = helper.getLong();
                    return result;
                });
            std::cout << "TBL_HIST_SMS count : " << count << std::endl;
            if(count > 1000){
                conn.directExecute("TRUNCATE TABLE TBL_HIST_SMS")
                    .fetch();
            }
        }

        {
            std::string query = "select count(*) from TBL_HIST_LMS";
            auto count = conn.directExecute(query)
                .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                    int result;
                    result = helper.getLong();
                    return result;
                });
            std::cout << "TBL_HIST_LMS count : " << count << std::endl;
            if(count > 1000){
                conn.directExecute("TRUNCATE TABLE TBL_HIST_LMS")
                    .fetch();
            }
        }



        {
            std::string query = "select count(*) from TBL_HIST_MMS";
            auto count = conn.directExecute(query)
                .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                    int result;
                    result = helper.getLong();
                    return result;
                });
            std::cout << "TBL_HIST_MMS count : " << count << std::endl;
            if(count > 1000){
                conn.directExecute("TRUNCATE TABLE TBL_HIST_MMS")
                    .fetch();
            }
        }

        std::this_thread::sleep_until(next_time);
    }

}  

