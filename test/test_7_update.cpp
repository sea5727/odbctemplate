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
    
    
    TSMS_HISTORY result;

    auto now = time(0);
    auto fmt = get_time_fmt(now);
    
    // update one line 
    auto count = 
    conn.preparedExecute("UPDATE TSMS_HISTORY SET SC_TIME=? where MSG_SEQ=?", fmt, 10590)
        .getUpdateRowCount();

    printf("count:%d\n", count);
    // reuse update handler

    auto stmt = conn.preparedStmt("UPDATE TSMS_HISTORY SET SC_TIME=? where MSG_SEQ=?");
    for(int i = 10591 ; i < 10595 ; i++){
        auto cnt = stmt.bindExecute(fmt, i).getRowCount();
        printf("count:%d\n", cnt);
    }

    return 0;

}

