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
    odbctemplate::Char<16 + 1> SC_TIME;
    odbctemplate::NullChar<16 + 1> SEND_TIME;
    odbctemplate::NullChar<16 + 1> IN_SIP_URI;
    
    void
    print(){
        printf("MSG_SEQ:%d, PROC_RESULT(%d:%d), IN_SECT(%d:%d), SC_TIME:%s, SEND_TIME(%d:%s), IN_SIP_URI(%d:%s)\n", 
            MSG_SEQ.Value,
            PROC_RESULT.Valid(), PROC_RESULT.Valid() == true ? PROC_RESULT.Value : 0,
            IN_SECT.Valid(), IN_SECT.Valid() == true ? IN_SECT.Value : 0,
            SC_TIME.Value,
            SEND_TIME.Valid(), SEND_TIME.Valid() == true ? SEND_TIME.Value: "", 
            IN_SIP_URI.Valid(), IN_SIP_URI.Valid() == true ? IN_SIP_URI.Value: "");
    }
};

std::string
get_time_fmt(time_t & t){
    char buff[20];
    strftime(buff, sizeof(buff), "%Y%m%d%H%M%S", localtime(&t));
    return buff;
}

int main(int argc, char* argv[]) {
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(true)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();
    
    

    auto now = time(0);
    auto fmt = get_time_fmt(now);
    

    TSMS_HISTORY result;

    auto fetcher = 
    conn.directExecute("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY;");

    while(fetcher.fetch()){
        fetcher.getData(1, &result.MSG_SEQ);
        fetcher.getData(2, &result.PROC_RESULT);
        fetcher.getData(3, &result.IN_SECT);
        fetcher.getData(4, &result.SC_TIME);
        fetcher.getData(5, &result.SEND_TIME);
        fetcher.getData(6, &result.IN_SIP_URI);    
        result.print();
    }

    


    return 0;

}

