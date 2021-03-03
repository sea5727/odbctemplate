#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate.hpp"

class TSMS_HISTORY{
public:
    odbctemplate::Int64 MSG_SEQ;
    odbctemplate::NullInt64 PROC_RESULT;
    odbctemplate::NullInt64 IN_SECT;
    odbctemplate::String SC_TIME;
    odbctemplate::NullString SEND_TIME;
    odbctemplate::NullString IN_SIP_URI;
    void
    print(){
        printf("MSG_SEQ:%d, PROC_RESULT:%d, IN_SECT:%d, SC_TIME:%s, SEND_TIME:%s, IN_SIP_URI:%s\n", 
                MSG_SEQ.Value, 
                PROC_RESULT.Valid() == true ? PROC_RESULT.Value : 0,
                IN_SECT.Valid() == true ? IN_SECT.Value : 0,
                SC_TIME.Value.c_str(),
                SEND_TIME.Valid() == true ? SEND_TIME.Value.c_str() : "",
                IN_SIP_URI.Valid() == true ? IN_SIP_URI.Value.c_str() : "");
    }
};


int main(int argc, char* argv[]) {
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(true)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();
    
    // reuse stmt handler
    auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ = ?;");

    for(int i = 10590 ; i < 10631; i++){
        auto ret = stmt.bindExecute(i)
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
        for(auto & result : ret){
            result.print();
        }
    }




    

    return 0;

}

