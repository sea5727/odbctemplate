#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>
#include <fstream>

#include "odbctemplate.hpp"



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
insertMsgN(
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
    int interval_sec = 5;
    int insert_count = 1000;


    auto fileid_key = rand;
    auto fileid_index = 0;

    printf("fileid_key:%lu\n", fileid_key);

    auto conn = odbctemplate::OdbcConnectBuilder()
        .setDsn("DSN=RCS_DSN_NEW;UID=rcs;PWD=rcs.123;")
        .setAutocommit(true)
        .setLoginTimeout(10)
        .build();


    auto now = std::chrono::system_clock::now();
    auto p = std::chrono::system_clock::time_point(now);
    auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);
    auto next_time = start_time + std::chrono::seconds(1);
    std::this_thread::sleep_until(next_time);
    
    
    std::ifstream fin("./tbl_send_file.sql");
    std::ifstream sin("./tbl_send_sms.sql");
    std::ifstream lin("./tbl_send_lms.sql");
    std::ifstream min("./tbl_send_mms.sql");

    std::stringstream ss_file_insert_sql;
    std::stringstream ss_sms_insert_sql;
    std::stringstream ss_lms_insert_sql;
    std::stringstream ss_mms_insert_sql;
    ss_file_insert_sql << fin.rdbuf();
    ss_sms_insert_sql << sin.rdbuf();
    ss_lms_insert_sql << lin.rdbuf();
    ss_mms_insert_sql << min.rdbuf();

    auto file_insert_sql = ss_file_insert_sql.str();
    auto sms_insert_sql = ss_sms_insert_sql.str();
    auto lms_insert_sql = ss_lms_insert_sql.str();
    auto mms_insert_sql = ss_mms_insert_sql.str();

    auto insert_file_stmt = conn.preparedStmt("INSERT INTO TBL_SEND_FILE(RCS_ID, FILE_ID, FILE_NAME, FILE_SIZE, MIMETYPE, ORDER_FLAG) VALUES ('clientId001' , ? , '/opt/img_1_1mb.jpg', 0, 'image/jpeg', 0);");

    while(1){

        
        auto now = std::chrono::system_clock::now();

        std::cout << "[START] ===== " << currentDateTime() << std::endl; 
        auto p = std::chrono::system_clock::time_point(now);
        auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);

        auto next_time = start_time + std::chrono::seconds(interval_sec);

        {
            std::string table = "TBL_SEND_FILE";
            auto count = selectCount(conn, table);
            
            if(count < limit){
                std::cout << "[" << table <<"]" << " count : " << count << std::endl;
                auto succ_count = insertFileN(insert_file_stmt, insert_count, fileid_key, fileid_index);
                std::cout << "Insert try : " << insert_count << ", Success : " << succ_count << std::endl;
            }
        }

        {
            std::string table = "TBL_SEND_SMS";
            auto count = selectCount(conn, table);
            
            if(count < limit){
                std::cout << "[" << table <<"]" << " count : " << count << std::endl;
                auto succ_count = insertMsgN(conn, insert_count, sms_insert_sql);
                std::cout << "[" << table << "] Insert try : " << insert_count << ", Success : " << succ_count << std::endl;
            }
        }

        {
            std::string table = "TBL_SEND_LMS";
            auto count = selectCount(conn, table);
            
            if(count < limit){
                std::cout << "[" << table <<"]" << " count : " << count << std::endl;
                auto succ_count = insertMsgN(conn, insert_count, sms_insert_sql);
                std::cout << "[" << table << "] Insert try : " << insert_count << ", Success : " << succ_count << std::endl;
            }
        }

        {
            std::string table = "TBL_SEND_MMS";
            auto count = selectCount(conn, table);
            
            if(count < limit){
                std::cout << "[" << table <<"]" << " count : " << count << std::endl;
                auto succ_count = insertMsgN(conn, insert_count, sms_insert_sql);
                std::cout << "[" << table << "] Insert try : " << insert_count << ", Success : " << succ_count << std::endl;
            }
        }
        std::this_thread::sleep_until(next_time);
    }
}

  

