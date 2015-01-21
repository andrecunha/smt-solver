#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "spec.h"
#include "data_structures.h"

// Represents a model. A model is an assignment of
//  values to variables of a specification.
class model
{
private:
    std::vector<variable*> m_variables;
public:
    model(spec *s, std::vector<constraint> c);

    // "toString"
    friend std::ostream &operator<<(std::ostream &out, const model *obj);
};

#endif // MODEL_H
