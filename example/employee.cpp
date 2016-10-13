#include <string>
#include <iostream>
#include "yarma.hpp"

using std::string;
using std::cout;
using std::visit;

struct employee {
    
    int id;
    string first_name;
    double hourly_wage;
    int weekly_hours;
    
    double weekly_earnings() const {
        return hourly_wage * weekly_hours;
    }
    
    YARMA_REFLECT(
        (id)
        (first_name)
        (hourly_wage)
        (weekly_hours)
        (weekly_earnings)
    )
};

int main () {
    
    auto e = employee{1234, "Bob", 7.25, 40};
    
    for (auto member : e.members()) {
        visit([&e](auto member) {
            cout << member.name() << " = " << member.get(e) << '\n';
        }, member);
    }
}