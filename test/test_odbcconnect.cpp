#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

// #include "odbctemplate.hpp"
#include "odbctemplate_all.hpp"

class TSMS_HISTORY{
public:
    odbctemplate::Int64 MSG_SEQ;
    odbctemplate::NullInt64 PROC_RESULT;
    odbctemplate::NullInt64 IN_SECT;
    odbctemplate::String SC_TIME;
    odbctemplate::NullString SEND_TIME;
    odbctemplate::NullString IN_SIP_URI;
    void
    print(int index){
        printf("[%d] MSG_SEQ:%d, PROC_RESULT:%d, IN_SECT:%d, SC_TIME:%s, SEND_TIME:%s, IN_SIP_URI:%s\n", 
            index,
            MSG_SEQ.Int64, 
            PROC_RESULT.Valid() == true ? PROC_RESULT.Int64 : 0,
            IN_SECT.Valid() == true ? IN_SECT.Int64 : 0,
            SC_TIME.String.c_str(),
            SEND_TIME.Valid() == true ? SEND_TIME.String.c_str() : "",
            IN_SIP_URI.Valid() == true ? IN_SIP_URI.String.c_str() : "");
    }
};

class TSMS_HISTORY2{
public:
    odbctemplate::Int64 MSG_SEQ;
    odbctemplate::NullInt64 PROC_RESULT;
    odbctemplate::NullInt64 IN_SECT;
    odbctemplate::Char<16 + 1> SC_TIME;
    odbctemplate::NullChar<16 + 1> SEND_TIME;
    odbctemplate::NullChar<16 + 1> IN_SIP_URI;
    
    void
    print(){
        printf("MSG_SEQ:%d, PROC_RESULT(%d:%d), IN_SECT(%d:%d), SC_TIME:%s, SEND_TIME(%d:%s), IN_SIP_URI(%d:%s)\n", 
            MSG_SEQ,
            PROC_RESULT.Valid(), PROC_RESULT.Valid() == true ? PROC_RESULT.Int64 : 0,
            IN_SECT.Valid(), IN_SECT.Valid() == true ? IN_SECT.Int64 : 0,
            SC_TIME.Char,
            SEND_TIME.Valid(), SEND_TIME.Valid() == true ? SEND_TIME.Char: "", 
            IN_SIP_URI.Valid(), IN_SIP_URI.Valid() == true ? IN_SIP_URI.Char: "");
    }
};

int main(int argc, char* argv[]) {
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(true)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();
    
    {
        // test : direct query 

        auto ret = 
        conn.directExecute("select * from TSMS_HISTORY limit 10;")
            .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
            TSMS_HISTORY ret;
            ret.MSG_SEQ = helper.getInt64();
            ret.PROC_RESULT = helper.getNullInt64();
            return ret;
        });

        printf("size:%d\n", ret.size());
    }

    {
        // test : binding query 
        auto ret = 
        conn.preparedExecute("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ > ?;", 10590)
            .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
            TSMS_HISTORY ret;
            ret.MSG_SEQ = helper.getInt64();
            ret.PROC_RESULT = helper.getNullInt64();
            ret.IN_SECT = helper.getNullInt64();
            ret.SC_TIME = helper.getString();
            ret.SEND_TIME = helper.getNullString();
            ret.IN_SIP_URI = helper.getNullString();
            return ret;
        });

        printf("size:%d\n", ret.size());
    }

    {
        // test : binding query and reuse preparedstmt handler
        int thredhold = 10590;
        int limit = 50;
        auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ > ? limit ?;");
        auto ret1 = stmt.bindExecute(thredhold, limit)
            .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
            TSMS_HISTORY ret;
            ret.MSG_SEQ = helper.getInt64();
            ret.PROC_RESULT = helper.getNullInt64();
            ret.IN_SECT = helper.getNullInt64();
            ret.SC_TIME = helper.getString();
            ret.SEND_TIME = helper.getNullString();
            ret.IN_SIP_URI = helper.getNullString();
            return ret;
        }, limit);

        thredhold = 10600;
        limit = 20;
        auto ret2 = stmt.bindExecute(thredhold, limit)
            .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
            TSMS_HISTORY ret;
            ret.MSG_SEQ = helper.getInt64();
            ret.PROC_RESULT = helper.getNullInt64();
            ret.IN_SECT = helper.getNullInt64();
            ret.SC_TIME = helper.getString();
            ret.SEND_TIME = helper.getNullString();
            ret.IN_SIP_URI = helper.getNullString();
            return ret;
        }, limit);

        printf("ret1 size:%d\n", ret1.size());
        printf("ret2 size:%d\n", ret2.size());
    }
    

    {
        TSMS_HISTORY2 ret;
        auto fetcher = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY")
            .Execute()
            .bindResultCol([&](odbctemplate::BindColHelper & helper) {
                helper.setBindColLongNotNull(&ret.MSG_SEQ.Int64);
                helper.setBindColLongNullable(&ret.PROC_RESULT);
                helper.setBindColLongNullable(&ret.IN_SECT);
                helper.setBindColStringNotNull(ret.SC_TIME.Char);
                helper.setBindColStringNullable(&ret.SEND_TIME);
                helper.setBindColStringNullable(&ret.IN_SIP_URI);
                
            });
            

        while(fetcher.fetch()){
            // ret.print();
        }
    }

    {
        //mutlithread test
        std::vector<std::thread> workers;
        workers.emplace_back([]{
            auto conn = odbctemplate::OdbcConnectBuilder()
                .setAutocommit(true)
                .setLoginTimeout(30)
                .setDsn("DSN=TST_DB;")
                .build();
            int thredhold = 10590;
            int limit = 50;
            auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ > ? limit ?;");

            for(int i = 0 ; i < 100 ; i++){
                auto ret1 = stmt.bindExecute(thredhold, limit)
                    .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
                    TSMS_HISTORY ret;
                    ret.MSG_SEQ = helper.getInt64();
                    ret.PROC_RESULT = helper.getNullInt64();
                    ret.IN_SECT = helper.getNullInt64();
                    ret.SC_TIME = helper.getString();
                    ret.SEND_TIME = helper.getNullString();
                    ret.IN_SIP_URI = helper.getNullString();
                    return ret;
                }, limit);
                
                for(auto & ret : ret1){
                    ret.print(1);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
        workers.emplace_back([]{
            auto conn = odbctemplate::OdbcConnectBuilder()
                .setAutocommit(true)
                .setLoginTimeout(30)
                .setDsn("DSN=TST_DB;")
                .build();
            int thredhold = 10590;
            int limit = 50;
            auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ > ? limit ?;");

            for(int i = 0 ; i < 100 ; i++){
                auto ret1 = stmt.bindExecute(thredhold, limit)
                    .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
                    TSMS_HISTORY ret;
                    ret.MSG_SEQ = helper.getInt64();
                    ret.PROC_RESULT = helper.getNullInt64();
                    ret.IN_SECT = helper.getNullInt64();
                    ret.SC_TIME = helper.getString();
                    ret.SEND_TIME = helper.getNullString();
                    ret.IN_SIP_URI = helper.getNullString();
                    return ret;
                }, limit);
                
                for(auto & ret : ret1){
                    ret.print(2);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
        workers.emplace_back([]{
            auto conn = odbctemplate::OdbcConnectBuilder()
                .setAutocommit(true)
                .setLoginTimeout(30)
                .setDsn("DSN=TST_DB;")
                .build();
            int thredhold = 10590;
            int limit = 50;
            auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ > ? limit ?;");

            for(int i = 0 ; i < 100 ; i++){
                auto ret1 = stmt.bindExecute(thredhold, limit)
                    .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
                    TSMS_HISTORY ret;
                    ret.MSG_SEQ = helper.getInt64();
                    ret.PROC_RESULT = helper.getNullInt64();
                    ret.IN_SECT = helper.getNullInt64();
                    ret.SC_TIME = helper.getString();
                    ret.SEND_TIME = helper.getNullString();
                    ret.IN_SIP_URI = helper.getNullString();
                    return ret;
                }, limit);
                
                for(auto & ret : ret1){
                    ret.print(3);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });

        for(auto & worker : workers){
            printf("join ? \n");
            worker.join();
        }

    }
    return 0;

}

