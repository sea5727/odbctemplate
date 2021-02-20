#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate.hpp"


int main(int argc, char* argv[]) {

    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(false)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();

    odbctemplate::OdbcConnectCtl::setAutocommit(conn, true);

    auto autocommit = odbctemplate::OdbcConnectCtl::getAutocommit(conn);
    auto logintimeout = odbctemplate::OdbcConnectCtl::getLoginTimeout(conn);
    printf("autocommit : %d\n", autocommit);
    printf("logintimeout : %d\n", logintimeout);

    return 0;

}
