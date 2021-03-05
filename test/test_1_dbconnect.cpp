#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>
#include <assert.h>

#include "odbctemplate.hpp"


int main(int argc, char* argv[]) {

    auto conn = odbctemplate::OdbcConnectBuilder()
        .build();

    // odbctemplate::OdbcConnectCtl::setAutocommit(conn, true);
    // odbctemplate::OdbcConnectCtl::setLoginTimeout(conn, 30);
    // odbctemplate::OdbcConnectCtl::connectDb(conn, "DSN=TST_DB;");
    
    // auto autocommit = odbctemplate::OdbcConnectCtl::getAutocommit(conn);
    // auto connection_dead = odbctemplate::OdbcConnectCtl::getConnectionDead(conn);
    // auto login_timeout = odbctemplate::OdbcConnectCtl::getLoginTimeout(conn);

    // assert(autocommit == true);
    // assert(login_timeout == 30);
    // assert(connection_dead == false);

    // odbctemplate::OdbcConnectCtl::setAutocommit(conn, false);
    // odbctemplate::OdbcConnectCtl::setLoginTimeout(conn, 10);

    // autocommit = odbctemplate::OdbcConnectCtl::getAutocommit(conn);
    // login_timeout = odbctemplate::OdbcConnectCtl::getLoginTimeout(conn);

    // assert(autocommit == false);
    // assert(login_timeout == 10);

    while(1){
        printf("while start\n");

        // auto connection_dead = odbctemplate::OdbcConnectCtl::getConnectionDead(conn);
        // printf("connection_dead:%d\n", connection_dead);
        if(true){
            try{
                odbctemplate::OdbcConnectCtl::connectDb(conn, "DSN=TST_DB;");
            } catch(odbctemplate::OdbcError & error){
                printf("erorr:%s\n", error.what());
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;

}

