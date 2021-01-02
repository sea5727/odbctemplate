#include <iostream>
#include <string>
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <typeinfo>

#include "odbctemplate.hpp"

class Base{
public:
    Base(){
        std::cout << "Default Base Constructor\n";
    }
    Base(const Base & copy)
        : _a{copy._a}
        , _b{copy._b} { 
        std::cout << "copy Constructor\n";
    }
    Base(Base && move)
        : _a{move._a}
        , _b{move._b} { 
        std::cout << "move Constructor\n";
    }
    ~Base(){
        std::cout << "Default Constructor\n";
    }    
    int _a;
    int _b;
    Base & seta(int a) { _a = a; return *this;}
    Base & setb(int b) { _b = b; return *this;}
    void print() { std::cout << "a:" << _a << " b:" << _b << std::endl; }

};


class Pizza {
 
private:
     
    class PizzaProperties {
         
    private:
         
        PizzaProperties() : fSize(0), 
                            fWithCheese(false), 
                            fWithPepperoni(false),
                            fWithBacon(false) {
        }
        
         
        int fSize;
         
        bool fWithCheese;
        bool fWithPepperoni;
        bool fWithBacon;
         
        friend class Pizza;
        friend class PizzaBuilder;
    };
     
     
public:
     
    class PizzaBuilder {
         
    public:
         
        PizzaBuilder(int size)  {
            fProperties.fSize = size;
        }
         
        PizzaBuilder& WithCheese() {
            fProperties.fWithCheese = true;
            return *this;
        }
         
        PizzaBuilder& WithPepperoni() {
            fProperties.fWithPepperoni = true;
            return *this;
        }
         
        PizzaBuilder& WithBacon() {
            fProperties.fWithBacon = true;
            return *this;
        }
         
        Pizza Build() {
            return Pizza(fProperties);
        }
         
    private:
         
        PizzaProperties fProperties;
    };
     
     
private:
     
    Pizza(const PizzaProperties& properties) : fProperties(properties) {
         std::cout <<"Pizza properties constructor\n";
    }

    Pizza(){
        std::cout <<"Pizza default constructor\n";
    }
    Pizza(const Pizza & copy){
        std::cout <<"Pizza copy constructor\n";
    }
    Pizza(Pizza && move){
        std::cout <<"Pizza move constructor\n";
    }
public:
    ~Pizza(){
        std::cout <<"Pizza delete\n";
    }

    PizzaProperties fProperties;
};


int main(int argc, char * argv[]){
    Base b;
    b.seta(5).setb(15);
    b.print();


    Pizza largeMargheritaPizza = 
        Pizza::PizzaBuilder(30).WithCheese().Build();
    
    Pizza smallHotPizza = 
        Pizza::PizzaBuilder(20).WithCheese().WithPepperoni().Build();



    auto conn = odbctemplate::OdbcConnect::OdbcConnectBuilder("DSN=TST_DB;")
        .setLoginTimeout(10)
        .setAutocommit(true)
        .build();

    // auto conn = odbctemplate::OdbcConnect::get_connection("DSN=TST_DB;").setAutoCommit(false);
    // auto & conn2 = conn.setAutoCommit(false);
    


    return 0;
    
}
