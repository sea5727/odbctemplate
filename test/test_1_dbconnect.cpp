#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>
#include <assert.h>

#include "odbctemplate_all.hpp"


int main(int argc, char* argv[]) {
    auto conn = odbctemplate::OdbcConnectBuilder()
        .setAutocommit(true)
        .setLoginTimeout(30)
        .setDsn("DSN=TST_DB;")
        .build();
    
    auto autocommit = odbctemplate::OdbcConnectCtl::getAutocommit(conn);
    auto connection_dead = odbctemplate::OdbcConnectCtl::getConnectionDead(conn);
    auto login_timeout = odbctemplate::OdbcConnectCtl::getLoginTimeout(conn);

    assert(autocommit == true);
    assert(login_timeout == 30);
    assert(connection_dead == false);

    odbctemplate::OdbcConnectCtl::setAutocommit(conn, false);
    odbctemplate::OdbcConnectCtl::setLoginTimeout(conn, 10);

    autocommit = odbctemplate::OdbcConnectCtl::getAutocommit(conn);
    login_timeout = odbctemplate::OdbcConnectCtl::getLoginTimeout(conn);

    assert(autocommit == false);
    assert(login_timeout == 10);

    return 0;

}

