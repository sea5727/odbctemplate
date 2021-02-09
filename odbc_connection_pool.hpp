#pragma once


#include "odbctemplate.hpp"

namespace odbctemplate
{
    class OdbcConnectionPool{
    private:
        std::string conn_info = "";
        unsigned int max_pool = 32;
        unsigned int min_pool = 4;
        unsigned int current_size = 4;
        std::vector<OdbcConnect> pool;
    public:
        OdbcConnectionPool() = default;
        OdbcConnectionPool(const std::string & conn_info, unsigned int min_pool, unsigned int max_pool)
            : conn_info(conn_info)
            , max_pool(max_pool) 
            , min_pool(min_pool)
            , current_size(min_pool) {
            
            //std::cout << "create\n";

            pool.reserve(max_pool);

            //std::cout << "reserve\n";

            for(unsigned int i = 0 ; i < min_pool ; ++i){
                //std::cout << "i:" << i << std::endl;
                // pool.emplace_back(OdbcConnect::get_connection(conn_info));
            }

            //std::cout << "end\n";
        }

        static
        OdbcConnectionPool
        get_connection_pool(const std::string & conn_info, unsigned int min_pool, unsigned int max_pool){
            return {conn_info, min_pool, max_pool};
        }

        OdbcConnect &
        get_connection_idx(const unsigned int index){
            if(index > max_pool)
                odbctemplate::OdbcError::Throw("Invalid Connection Pool index"); 
            //std::cout << "index : " << index << ", current_size:" << current_size << std::endl;

            if(current_size < index + 1){
                for(unsigned int i = current_size ; i < index + 1 ; ++i){
                    //std::cout << "i : " << i << std::endl;
                    // pool.emplace_back(OdbcConnect::get_connection(conn_info));
                }
                current_size = index + 1;
            }
            return pool[index]; // copy??
        }
        
        
        void
        put_connection(){
            
        }
        


    };
}