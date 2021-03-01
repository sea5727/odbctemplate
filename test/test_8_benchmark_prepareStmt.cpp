#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

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
    print(){
        printf("MSG_SEQ:%d, PROC_RESULT:%d, IN_SECT:%d, SC_TIME:%s, SEND_TIME:%s, IN_SIP_URI:%s\n", 
                MSG_SEQ.Int64, 
                PROC_RESULT.Valid() == true ? PROC_RESULT.Int64 : 0,
                IN_SECT.Valid() == true ? IN_SECT.Int64 : 0,
                SC_TIME.String.c_str(),
                SEND_TIME.Valid() == true ? SEND_TIME.String.c_str() : "",
                IN_SIP_URI.Valid() == true ? IN_SIP_URI.String.c_str() : "");
    }
};


int main(int argc, char* argv[]) {
    if(argc < 2){
        std::cout << "input max count" << std::endl;
        return 0;
    }

    auto max = atoi(argv[1]);
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(true)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();

    
    auto stmt = conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY;");
    for(int i = 0 ; i < max ; i++){
        auto ret = 
        stmt.Execute()
            .fetch<TSMS_HISTORY>([](odbctemplate::FetchHelper & helper){
            TSMS_HISTORY ret;
            ret.MSG_SEQ = helper.getInt64();
            ret.PROC_RESULT = helper.getNullInt64();
            ret.IN_SECT = helper.getNullInt64();
            ret.SC_TIME = helper.getString();
            ret.SEND_TIME = helper.getNullString();
            ret.IN_SIP_URI = helper.getNullString();
            return ret;
        },68);
    }

    
    return 0;

}
