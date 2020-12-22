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
    if(argc < 2){
        std::cout << "param  maximum data  " << std::endl;
        return 0;
    }

    auto rand = time_t(argv[2]);

    int limit = std::stoi(argv[1]);
    int interval_sec = 5;
    int insert_count = 1000;


    auto fileid_key = rand;
    auto fileid_index = 0;

    auto conn = odbctemplate::OdbcConnect::get_connection("DSN=RCS_DSN_NEW;UID=rcs;PWD=rcs.123;");


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

    
    while(1){

        
        auto now = std::chrono::system_clock::now();

        std::cout << "[START] ===== " << currentDateTime() << std::endl; 
        auto p = std::chrono::system_clock::time_point(now);
        auto start_time = std::chrono::time_point_cast<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>::duration> (p);

        auto next_time = start_time + std::chrono::seconds(interval_sec);

        {
            std::string query = "select count(*) from TBL_SEND_FILE";
            auto count = conn.directExecute(query)
                .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                    int result;
                    result = helper.getLong();
                    return result;
                });

            if(count < limit){
                std::cout << "TBL_SEND_FILE count : " << count << std::endl;
                char fileid[64] = {0,};
                for(int i = 0 ; i < insert_count ; ++i ){
                    std::string query =  "INSERT INTO TBL_SEND_FILE(RCS_ID, FILE_ID, FILE_NAME, FILE_SIZE, MIMETYPE, ORDER_FLAG) VALUES ('clientId001' , ? , '/opt/img_1_1mb.jpg', 0, 'image/jpeg', 0);";
                    sprintf(fileid, "clientId001_file_%d_%016d", fileid_key, fileid_index);
                    fileid_index += 1;
                    
                    conn.preparedStmt(query).bindExecute(fileid)
                        .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                            int result;
                            result = helper.getLong();
                            return result;
                        });
                }
            }
        }


        {
            std::string query = "select count(*) from tbl_send_sms";
            auto count = conn.directExecute(query)
                .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                    int result;
                    result = helper.getLong();
                    return result;
                });

            if(count < limit){
                std::cout << "TBL_SEND_SMS count : " << count << std::endl;
                for(int i = 0 ; i < insert_count ; ++i ){
                    conn.directExecute(sms_insert_sql)
                        .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                            int result;
                            result = helper.getLong();
                            return result;
                        });
                }
            }
        }

        {
            std::string query = "select count(*) from TBL_SEND_LMS";
            auto count = conn.directExecute(query)
                .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                    int result;
                    result = helper.getLong();
                    return result;
                });

            if(count < limit){
                std::cout << "TBL_SEND_LMS count : " << count << std::endl;
                for(int i = 0 ; i < insert_count ; ++i ){
                    conn.directExecute(lms_insert_sql)
                        .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                            int result;
                            result = helper.getLong();
                            return result;
                        });
                }
            }
        }
        {
            std::string query = "select count(*) from TBL_SEND_MMS";
            auto count = conn.directExecute(query)
                .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                    int result;
                    result = helper.getLong();
                    return result;
                });

            if(count < limit){
                std::cout << "TBL_SEND_MMS count : " << count << std::endl;
                for(int i = 0 ; i < insert_count ; ++i ){
                    conn.directExecute(mms_insert_sql)
                        .fetchOne<int>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                            int result;
                            result = helper.getLong();
                            return result;
                        });
                }
            }
        }




        

        std::this_thread::sleep_until(next_time);
    }

}  

