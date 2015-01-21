#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "data_structures.h"

std::string type_names[1] = {"Bool"};
std::string op_names[6] = {"or", "and", "not", "=>", "="};

/* bool_variable definitions */

bool_variable::bool_variable(std::string name) {
    has_value_set = false;
    m_name = name;
}

bool_variable::bool_variable(std::string name, bool value)
{
    has_value_set = true;
    m_name = name;
    m_value = value;
}

std::string bool_variable::name() const {
    return m_name;
}

type_t bool_variable::type() const {
    return bool_t;
}

bool bool_variable::value() {
    return m_value;
}

void bool_variable::set_value(bool value)
{
    m_value = value;
}

std::ostream &operator<<(std::ostream &out, const variable *obj)
{
    out << "(" << obj->name() << " " << type_names[obj->type()] << ")";
    return out;
}

const bool_variable *bool_variable::TRUE = new bool_variable("1", true);
const bool_variable *bool_variable::FALSE = new bool_variable("0", false);


constraint::constraint(list *exp, bool v)
{
    expression = exp;
    value = v;
    expanded = false;

    // Now, we check how many branches this constraint
    //  generates.

    if (exp->type == NODE) {
        required_branches = 1;
        return;
    }

    std::string op = *(exp->content.sublist->at(0)->content.name);
    int nargs = exp->content.sublist->size() - 1;

    if(!op.compare("or")) {
#ifdef DEBUG
        std::cout << "Got an or. ";
#endif

        if (v) {
            // If an or expression is true, at least one
            //  of its arguments is true.
            required_branches = nargs;
        } else {
            // If an or expression is false, all of its
            //  arguments are false.
            required_branches = 1;
        }
    } else if(!op.compare("and")) {
#ifdef DEBUG
        std::cout << "Got an and. ";
#endif

        if (v) {
            // If an and expression is true, all of its
            //  arguments are true.
            required_branches = 1;
        } else {
            // If an and expression is false, at least one
            //  of its arguments is false.
            required_branches = nargs;
        }
    } else if(!op.compare("not")) {
#ifdef DEBUG
        std::cout << "Got a not. ";
#endif

        required_branches = 1;
    } else if(!op.compare("=>")) {
#ifdef DEBUG
        std::cout << "Got a =>. ";
#endif

        if (v) {
            // If an implication P => Q is true, then
            //  either P is false or P == Q == true.
            required_branches = 2;
        } else {
            // If an implication P => Q is false, then
            //  P == false and Q == true.
            required_branches = 1;
        }
    } else if(!op.compare("=")) {
#ifdef DEBUG
        std::cout << "Got a =. ";
#endif

        if (v) {
            // If an equivalence P = Q is true, then
            //  either P == Q == true or P == Q == false.
            required_branches = 2;
        } else {
            // If an equivalence P = Q is false, then
            //  either P == false and Q == true or
            //  P == true and Q == false.
            required_branches = 2;
        }
    }

#ifdef DEBUG
    std::cout << "Required branches: " << required_branches << "." << std::endl;
#endif
}

std::ostream &operator<<(std::ostream &out, const constraint obj)
{
    out << obj.expression << " = " << obj.value;
    return out;
}
