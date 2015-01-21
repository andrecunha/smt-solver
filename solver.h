#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include "spec.h"
#include "model.h"
#include "data_structures.h"

// A branch is a list of constraints.
class branch
{
public:
    std::vector<constraint> constraints;
    // We keep a list of atomic constraints, which are
    //  constraints of the form (variable, value), to
    //  speed up the verification of closed branches.
    std::vector<constraint> atomic_constraints;

    branch();

    branch(const branch &other);

    void add_constraint(constraint c);

    void add_constraints(std::vector<constraint> cs);

    friend std::ostream &operator<<(std::ostream &out, const branch obj);
};

// Determines the satisfiability of a spec and, if it's sat,
//  returns a model that satisfies it, using the tableau method.
class solver
{
private:
    spec *m_spec;
    std::vector<branch> m_branches;

    // Chooses the expression that will create
    //  the fewest branches.
    int get_expression_to_branch(branch b);

    // Check if there are conflicting constraints
    //  over the same variable.
    bool branch_is_closed(branch b);

    // Gets the new constraints that must be satisfied
    //  in order to satisfy c.
    std::vector<std::vector<constraint> > get_branches(constraint c);
public:
    solver(spec *base_spec);
    bool check_sat();
    model *get_model();
};

#endif // SOLVER_H
