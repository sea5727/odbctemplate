#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>
#include <fstream>

#include "odbctemplate/odbctemplate.hpp"



const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


int
selectCount(
    odbctemplate::OdbcConnect & conn,
    const std::string & table){

    std::string query = "select count(*) from " + table;
    auto result = conn.directExecute(query)
        .fetch<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
            int result;
            result = helper.getLong();
            return result;
        });

    if(result.size() == 0)
        return 0;
    auto count = result.at(0);
    return count;
}

int 
insertFileN(
    odbctemplate::OdbcpreparedStmt & stmt, 
    int insert_count, 
    unsigned long fileid_key, 
    int & fileid_index) {

    char fileid[64] = {0,};
    int succ_count = 0;
    for(int i = 0 ; i < insert_count ; ++i ){
        sprintf(fileid, "clientId001_file_%lu_%08d", fileid_key, fileid_index);
        succ_count += stmt.bindExecute(fileid).getInsertRowCount();
    }
    return succ_count;
}
int 
insertN(
    odbctemplate::OdbcConnect & conn, 
    int insert_count,
    const std::string & query ) {

    char fileid[64] = {0,};
    int succ_count = 0;
    for(int i = 0 ; i < insert_count ; ++i ){
        succ_count += conn.directExecute(query).getInsertRowCount();
    }
    return succ_count;
}

int main(int argc, char * argv[]) {
    if(argc < 2){
        std::cout << "param  maximum data  " << std::endl;
        return 0;
    }

    std::time_t rand = std::time(0);   // get time now


    int limit = std::stoi(argv[1]);
    int interval_sec = 1;
    int insert_count = 1000;


    auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=TST_DB;")
        .setAutocommit(true)
        .setLoginTimeout(10)
        .build();

    auto now = std::chrono::system_clock::now();
    auto p = std::chrono::system_clock::time_point(now);
    auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);
    auto next_time = start_time + std::chrono::seconds(1);
    std::this_thread::sleep_until(next_time);
    

    std::string table = "tuto";
    while(1){
        
        auto now = std::chrono::system_clock::now();

        std::cout << "[START] ===== " << currentDateTime() << std::endl; 
        auto p = std::chrono::system_clock::time_point(now);
        auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);

        auto next_time = start_time + std::chrono::seconds(interval_sec);


        auto count = selectCount(conn, table);
        std::cout << "count " << count << std::endl;
        if(count < limit){
            auto succ = insertN(conn, 100, "insert into tuto(name, test, address) VALUES ('sangho', 'testsample', 'addresssample')");
            std::cout << "insert succ : " << succ << std::endl;
        }
        std::this_thread::sleep_until(next_time);
    }
}

  

