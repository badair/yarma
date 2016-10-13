# yarma <a target="_blank" href="http://melpon.org/wandbox/permlink/JzmUX3xfdwHdNby9">![Try it online][badge.wandbox]</a>
Yet Another Reflection Macro: simple, flexible reflection in C++17.

Yarma was designed for ease-of-use over performance, but it's not terribly slow either. Yarma depends on a working implementation of `std::variant`, Boost (for Boost.Preprocessor), and my own [CallableTraits](https://github.com/badair/callable_traits) library, which is currently seeking a Boost review manager.

Once you have the dependencies installed, you only need a single header file, `yarma.hpp`. The `YARMA_REFLECT` macro simply defines a member function named "members" that returns an array of custom variant types, which can then be inspected.

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
        // could simply call m.get(emp) instead. The get function
        // speaks std::invoke, so you can pass arguments to member
        // functions through it too, as long as your reflected members
        // have covariant signatures. If not, you can do your own
        // SFINAE magic in a visitor using decltype(mem)::parameters,
        // which aliases a std::tuple containing the member's std::invoke
        // signature (where the first element is a reference to the
        // parent object).You can also use decltype(mem)::return_type
        // to inspect the result of std::invoke without needing to
        // decltype it.
        visit([=](auto mem) { cout << mem.get(emp); }, m);
        
        // is_data returns true for data members
        if(m.is_data())
            cout << " (data) ";   
        else
            // arity returns -1 for data members
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
You can run this code using the "Try it online" button at the top of this readme.
[badge.Wandbox]: https://img.shields.io/badge/try%20it-online-blue.svg
