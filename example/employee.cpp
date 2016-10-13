#include <string>
#include <iostream>
#include "yarma.hpp"

using std::string;
using std::cout;
using std::visit;

struct employee {
    
    int id;
    string name;
    double hourly_wage;
    int weekly_hours;
    
    double weekly_earnings() const {
        return hourly_wage * weekly_hours;
    }
    
    YARMA_REFLECT(
        (id)
        (name)
        (hourly_wage)
        (weekly_hours)
        (weekly_earnings)
    )
};

int main () {
    
    auto emp = employee{1234, "John Doe", 7.25, 40};
    
    for (auto m : emp.members()) {
        
        std::cout << m.name() << " = ";
        
        visit([=](auto m) { cout << m.get(emp); }, m);
        
        if(m.is_data())
            std::cout << " (data) ";   
        else
            std::cout << " (function taking " << m.arity() << " arguments) ";
        
        std::cout << '\n';
    }
}