#include <iostream>
#include "model.h"

model::model(spec *s, std::vector<constraint> c)
{
    m_variables = s->variables();

    std::vector<constraint>::iterator it;
    for (it = c.begin(); it != c.end(); it++) {
        constraint c2 = (*it);

        std::vector<variable*>::iterator it2;
        for (it2 = m_variables.begin(); it2 != m_variables.end(); it2++) {
            variable *var = (*it2);

            if (!c2.expression->content.name->compare(var->name().c_str())) {
                var->has_value_set = true;

                switch (var->type()) {
                case bool_t:
                {
                    bool_variable *v = (bool_variable*) var;
                    v->set_value(c2.value);
                }
                    break;
                default:
                    break;
                }
            }
        }
    }
}

std::ostream &operator<<(std::ostream &out, const model *obj)
{
    out <<  "Model" << std::endl;

    if (obj == 0) {
        out << "    (null)";
        return out;
    }

    std::vector<variable*>::const_iterator it;
    for(it = obj->m_variables.begin(); it != obj->m_variables.end();) {
        variable *var = *(it++);

        if (var->has_value_set) {
            out << "    " << var << " = ";

            switch (var->type()) {
            case bool_t:
            {
                bool_variable *v = (bool_variable*) var;
                out << v->value();
            }
                break;
            default:
                break;
            }

            if (it != obj->m_variables.end()) {
                out << std::endl;
            }
        }
    }

    return out;
}
