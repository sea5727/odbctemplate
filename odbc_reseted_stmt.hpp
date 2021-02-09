#pragma once

#include "odbctemplate.hpp"

namespace odbctemplate
{
    
    class OdbcResetedStmt{
    private:
        std::shared_ptr<OdbcStmt> stmt;
    public:
        explicit OdbcResetedStmt() = default;
        explicit OdbcResetedStmt(SQLHSTMT stmt) 
            : stmt{std::make_shared<OdbcStmt>(stmt)} {
            //std::cout << "OdbcResetedStmt create..\n";
        }
        explicit OdbcResetedStmt(std::shared_ptr<OdbcStmt> stmt) 
            : stmt{stmt} {
            //std::cout << "OdbcResetedStmt create..\n";
        }
        OdbcResetedStmt(const OdbcResetedStmt & copy)
            : stmt{copy.stmt} {
            //std::cout << "OdbcResetedStmt copy create..\n";
        }
        explicit OdbcResetedStmt(OdbcResetedStmt && move) 
            : stmt{std::move(move.stmt)} {
            //std::cout << "OdbcResetedStmt move create..\n";
        }
        ~OdbcResetedStmt(){
            //std::cout << "OdbcResetedStmt delete..\n";
        }
 
    public:
        OdbcpreparedStmt
        preparedStmt(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            SQLRETURN status = SQLPrepare( stmt->stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            //std::cout << "======== " << __func__ << " end ============" << std::endl;
            return OdbcpreparedStmt{stmt};
        }
        
        OdbcFetcher
        directExecute(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;
            SQLRETURN status = SQLExecDirect(stmt->stmt, (SQLCHAR *)query.c_str(), query.length());
            if( status != SQL_SUCCESS){
                odbctemplate::OdbcError::Throw(SQL_HANDLE_STMT, stmt->stmt, status);
            }
            return OdbcFetcher{stmt};
        }


        OdbcFetcher 
        preparedExecute(const std::string & query){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;           
            return preparedStmt(query).Execute();
        }


        template <typename Param1, typename... Params>
        OdbcFetcher 
        preparedExecute(const std::string & query, const Param1 & p1, const Params&... rest){
            //std::cout << "======== " << __func__ << " start ============" << std::endl;           
            return preparedStmt(query).bindExecute(p1, rest...);
        }



        void
        print_mystmt(){
            //std::cout << "print_mystmt OdbcprepareStmt, " << stmt->stmt << std::endl;
        }
    };
}