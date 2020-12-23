#pragma once


#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

#include "odbc_stmt.hpp"


namespace odbctemplate
{
    class OdbcBindedColStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcBindedColStmt() = default;
        explicit OdbcBindedColStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{
            std::cout << "OdbcBindedColStmt default create..\n";
        }
        explicit OdbcBindedColStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} {
            std::cout << "OdbcBindedColStmt shared create..\n";
        }
        OdbcBindedColStmt(const OdbcBindedColStmt & copy) 
            : stmt{copy.stmt}{
            std::cout << "OdbcBindedColStmt copy create..\n";
        }
        explicit OdbcBindedColStmt(OdbcBindedColStmt && move) 
            : stmt{std::move(move.stmt)}{
            std::cout << "OdbcBindedColStmt move create..\n";
        }
        ~OdbcBindedColStmt(){
            std::cout << "OdbcBindedColStmt delete..\n";
        }


    };
}