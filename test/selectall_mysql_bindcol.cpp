#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>
#include <fstream>

#include "odbctemplate.hpp"



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
        .fetch<int>([](odbctemplate::FetchHelper helper){
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

class tuto2 {
public:
    long id;
    char name[64 + 1];
    char test[64 + 1];
    char address[128 + 1];
    void 
    print(){
        std::cout << "id : " << id << std::endl;
        std::cout << "name : " << name << std::endl;
        std::cout << "test : " << test << std::endl;
        std::cout << "address : " << address << std::endl;
    }
};
int main(int argc, char * argv[]) {


    std::time_t rand = std::time(0);   // get time now

    int interval_sec = 1;
    int insert_count = 1000;


    auto conn = odbctemplate::OdbcConnectBuilder()  
        .setDsn("DSN=TST_DB;")
        .setAutocommit(true)
        .setLoginTimeout(10)
        .build();

    auto now = std::chrono::system_clock::now();
    auto p = std::chrono::system_clock::time_point(now);
    auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);
    auto next_time = start_time + std::chrono::seconds(1);
    std::this_thread::sleep_until(next_time);
    
    tuto2 result;
    
    auto preparedStmt = conn
            .preparedStmt("select id, name, test, address from tuto")
            .bindCol([&](odbctemplate::BindColHelper helper){
                helper.setBindColLongNotNull(&result.id);
                helper.setBindColStringNotNull(result.name, sizeof(result.name));
                helper.setBindColStringNotNull(result.test, sizeof(result.test));
                helper.setBindColStringNotNull(result.address, sizeof(result.address));
            });
    std::string table = "tuto";

    

    while(1){
        
        auto now = std::chrono::system_clock::now();

        auto p = std::chrono::system_clock::time_point(now);
        auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);

        auto next_time = start_time + std::chrono::seconds(interval_sec);


        auto start = std::chrono::system_clock::now();

        auto count = selectCount(conn, table);
        int sel_count = 0;

        auto fetcher = preparedStmt.Execute();

        while(fetcher.fetch()){
            sel_count++;
        }
        
        std::cout << "count " << count << ", select count : " << sel_count << std::endl;
        auto end = std::chrono::system_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;


        std::this_thread::sleep_until(next_time);
    }
}

  

