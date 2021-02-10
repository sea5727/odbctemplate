#pragma once



#include "odbc_stmt.hpp"


namespace odbctemplate
{
    class OdbcBindedColStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcBindedColStmt() = default;
        explicit OdbcBindedColStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)}{ }
        explicit OdbcBindedColStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} { }
        OdbcBindedColStmt(const OdbcBindedColStmt & copy) 
            : stmt{copy.stmt}{ }
        explicit OdbcBindedColStmt(OdbcBindedColStmt && move) 
            : stmt{std::move(move.stmt)}{ }
        ~OdbcBindedColStmt(){ }


    };
}