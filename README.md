# yarma <a target="_blank" href="http://melpon.org/wandbox/permlink/huhA2GdFq64cM4Ya">![Try it online][badge.wandbox]</a>
Yet Another Reflection Macro: easy, portable, flexible reflection in C++17.

The `YARMA_REFLECT` macro simply defines a member function named "members" that returns an array of custom variant types, which can then be inspected.
```cpp
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
    
    // This macro does nothing more than define a "members" function
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
        
        cout << m.name() << " = ";
        
        // We use std::visit here because the reflected members
        // are not convertible to each other. If they were, we
        // could simply call m.get() instead
        visit([=](auto mem) { cout << mem.get(emp); }, m);
        
        if(m.is_data())
            cout << " (data) ";   
        else
            cout << " (function taking " << m.arity() << " arguments) ";
        
        cout << '\n';
    }
}
```

output:
```console
id = 1234 (data) 
name = John Doe (data) 
hourly_wage = 7.25 (data) 
weekly_hours = 40 (data) 
weekly_earnings = 290 (function taking 0 arguments) 
```
Yarma was designed for ease-of-use over performance, but it's not particularly slow either. Yarma depends on a working implementation of `std::variant`, Boost (for Boost.Preprocessor), and my own [CallableTraits](https://github.com/badair/callable_traits) library.

[badge.Wandbox]: https://img.shields.io/badge/try%20it-online-blue.svg
