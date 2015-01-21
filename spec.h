#ifndef SPEC_H
#define SPEC_H

#include <iostream>
#include <vector>
#include "data_structures.h"

// Represents a specification (spec). A spec is a set of variables and
//  a set of assertions about them. Variables are created using the
//  declare-const function, and assertions, using the assert function.
class spec
{
private:
    bool m_is_sat;
    std::vector<variable*> m_vars;
    std::vector<list*> m_assertions;
public:
    spec();
    void set_sat(bool sat);
    void add_variable(variable *var);
    void add_constraint(list *l);

    std::vector<list*> constraints();
    std::vector<variable*> variables();

    friend std::ostream &operator<<(std::ostream &out, const spec *obj);
};

#endif // SPEC_H
