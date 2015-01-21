#include <iostream>
#include "spec.h"

spec::spec()
{
}

void spec::set_sat(bool sat)
{
    m_is_sat = sat;
}

void spec::add_variable(variable *var)
{
    m_vars.push_back(var);
}

void spec::add_constraint(list *l)
{
    //TODO: verificar se todas as variáveis já foram
    //  declaradas.
    m_assertions.push_back(l);
}

std::vector<list *> spec::constraints()
{
    return m_assertions;
}

std::vector<variable *> spec::variables()
{
    return m_vars;
}

std::ostream &operator<<(std::ostream &out, const spec *obj)
{
    out << "Spec" << std::endl;
    out << "Variables:" << std::endl;

    std::vector<variable*>::const_iterator it;
    for(it = obj->m_vars.begin();
        it != obj->m_vars.end();
        ) {
        out << "    " << *(it++);
        if (it != obj->m_vars.end()) {
            out << std::endl;
        }
    }

    out << std::endl << "Constraints:" << std::endl;

    std::vector<list*>::const_iterator it2;
    for(it2 = obj->m_assertions.begin();
        it2 != obj->m_assertions.end();
        ) {
        out << "    " << *(it2++);
        if (it2 != obj->m_assertions.end()) {
            out << std::endl;
        }
    }

    return out;
}
