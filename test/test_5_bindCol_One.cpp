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


int main(int argc, char* argv[]) {
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(true)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();
    
    
    TSMS_HISTORY result;

    auto stmt = 
    conn.preparedStmt("select MSG_SEQ, PROC_RESULT, IN_SECT, SC_TIME, SEND_TIME, IN_SIP_URI from TSMS_HISTORY where MSG_SEQ = ?;")
        .bindResultCol([&result](odbctemplate::BindColHelper & helper){
            helper.setInt64NotNull(&result.MSG_SEQ);
            helper.setInt64Nullable(&result.PROC_RESULT);
            helper.setInt64Nullable(&result.IN_SECT);
            helper.setCharNotNull(&result.SC_TIME);
            helper.setCharNullable(&result.SEND_TIME);
            helper.setCharNullable(&result.IN_SIP_URI);
        })
        .bindExecute(10590)
        .fetch();

    result.print();

    return 0;

}

