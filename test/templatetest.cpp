#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate.hpp"

template<typename T>
class tuto {
public:
    int id;
    std::string name;
    std::string test;
    T generic;
    tuto(int id, std::string name, std::string test, T generic)
        : id(id)
        , name(name)
        , test(test)
        , generic(generic) {

    }

    void print(){
        std::cout << "id:" << id << ",name:" << name << ",test:" << test<< ", generic : " << generic << std::endl;
    }
};


template<typename Return_Ty, int N>
std::vector<Return_Ty>
fetch(std::function<Return_Ty()> help) {
    help();
    std::vector<Return_Ty> results;
    return results;
}




int main(int, char**) {
    tuto a(10, "name10", "test10", 1);
    tuto b(10, "name10", "test10", "generic");
    tuto c(10, "name10", "test10", "generic");

    a.print();
    b.print();
    c.print();
    std::vector<std::string> ret = fetch<std::string, 100>([](){
        return "test";
    });

    std::vector<int> ret2 = fetch<int, 1>([](){
        return 10;
    });

    // int ret3 = fetch<int>([](){
    //     return 10;
    // });
    return 0;
}
