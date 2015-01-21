#include <limits>
#include "solver.h"

branch::branch()
{

}

branch::branch(const branch &other)
{
    constraints = other.constraints;
    atomic_constraints = other.atomic_constraints;
}

void branch::add_constraint(constraint c)
{
    list *exp = c.expression;

#ifdef DEBUG
    std::cout << "Adding constraint: " << exp << std::endl;
#endif

    if (exp->type == NODE) {
#ifdef DEBUG
        std::cout << "    The constraint is atomic." << std::endl;
#endif
        c.expanded = true;
        atomic_constraints.push_back(c);
    }

    constraints.push_back(c);
}

void branch::add_constraints(std::vector<constraint> cs)
{
    for(unsigned int i = 0; i < cs.size(); i++) {
        add_constraint(cs[i]);
    }
}

std::ostream &operator<<(std::ostream &out, const branch obj)
{
    out << "Branch" << std::endl;
    out << "    Constraints: ";

    std::vector<constraint>::const_iterator it;
    for(it = obj.constraints.begin();
        it != obj.constraints.end();
        it++) {
        if (it != obj.constraints.end()) {
            out << *it << ", ";
        } else {
            out << *it;
        }
    }

    out << std::endl << "    Atomic constraints: ";

    for(it = obj.atomic_constraints.begin();
        it != obj.atomic_constraints.end();
        it++) {
        if (it != obj.atomic_constraints.end()) {
            out << *it << ", ";
        } else {
            out << *it;
        }
    }

    return out;
}

int solver::get_expression_to_branch(branch b)
{
    int min_branches = std::numeric_limits<int>::max();
    int const_index = -1;

    for(unsigned int i = 0; i < b.constraints.size(); i++) {
        constraint curr_constraint = b.constraints[i];

        if (curr_constraint.required_branches < min_branches
                && !curr_constraint.expanded
                && curr_constraint.expression->type == LIST) {
            min_branches = curr_constraint.required_branches;
            const_index = i;
        }

        if(min_branches == 1) {
            // We can't get anything better then 1 here,
            //  so we can stop as soon as we find it.
            break;
        }
    }

    return const_index;
}

bool solver::branch_is_closed(branch b)
{
    if (!b.atomic_constraints.size()) {
        // If there are no atomic constraits, the branch is open.
        return false;
    }

    for(unsigned int i = 0; i < b.atomic_constraints.size() - 1; i++) {
        for(unsigned int j = i + 1; j < b.atomic_constraints.size(); j++) {
            constraint v1 = b.atomic_constraints[i],
                    v2 = b.atomic_constraints[j];

            if(!(v1.expression->content.name->compare(
                        v2.expression->content.name->c_str()))
                    && (v1.value != v2.value)) {
                return true;
            }
        }
    }

    return false;
}

std::vector<std::vector<constraint> > solver::get_branches(constraint c)
{
    std::vector<std::vector<constraint> > branches;

    list *exp = c.expression;
    std::string op = *(exp->content.sublist->at(0)->content.name);
    int nargs = exp->content.sublist->size() - 1;
    bool v = c.value;

    if(!op.compare("or")) {
        if (v) {
            // If an or expression is true, at least one
            //  of its arguments is true.
            for (int i = 0; i < nargs; i++) {
                std::vector<constraint> curr_branch;

                list *curr_exp = exp->content.sublist->at(i + 1);

#ifdef DEBUG
                std::cout << "Will add constraint " << curr_exp << " = 1" << std::endl;
#endif

                constraint curr_c(curr_exp, true);
                curr_branch.push_back(curr_c);

                branches.push_back(curr_branch);
            }
        } else {
            // If an or expression is false, all of its
            //  arguments are false.
            std::vector<constraint> curr_branch;
            for (int i = 0; i < nargs; i++) {
                list *curr_exp = exp->content.sublist->at(i + 1);

#ifdef DEBUG
                std::cout << "Will add constraint " << curr_exp << " = 0" << std::endl;
#endif

                constraint curr_c(curr_exp, false);
                curr_branch.push_back(curr_c);
            }
            branches.push_back(curr_branch);
        }
    } else if(!op.compare("and")) {
        if (v) {
            // If an and expression is true, all of its
            //  arguments are true.
            std::vector<constraint> curr_branch;
            for (int i = 0; i < nargs; i++) {
                list *curr_exp = exp->content.sublist->at(i + 1);

#ifdef DEBUG
                std::cout << "Will add constraint " << curr_exp << " = 1" << std::endl;
#endif

                constraint curr_c(curr_exp, true);
                curr_branch.push_back(curr_c);
            }
            branches.push_back(curr_branch);

        } else {
            // If an and expression is false, at least one
            //  of its arguments is false.
            for (int i = 0; i < nargs; i++) {
                std::vector<constraint> curr_branch;

                list *curr_exp = exp->content.sublist->at(i + 1);

#ifdef DEBUG
                std::cout << "Will add constraint " << curr_exp << " = 0" << std::endl;
#endif

                constraint curr_c(curr_exp, false);
                curr_branch.push_back(curr_c);

                branches.push_back(curr_branch);
            }
        }
    } else if(!op.compare("not")) {
        std::vector<constraint> curr_branch;
        list *curr_exp = exp->content.sublist->at(1);

#ifdef DEBUG
        std::cout << "Will add constraint " << curr_exp << " = " << !v << std::endl;
#endif

        constraint curr_c(curr_exp, !v);
        curr_branch.push_back(curr_c);
        branches.push_back(curr_branch);
    } else if(!op.compare("=>")) {
        if (v) {
            // If an implication P => Q is true, then
            //  either P is false or P == Q == true.
            std::vector<constraint> branch1, branch2;
            list *exp1 = exp->content.sublist->at(1);
            list *exp2 = exp->content.sublist->at(2);

#ifdef DEBUG
            std::cout << "Will add constraint " << exp1 << " = 0 on branch 1" << std::endl;
#endif

            constraint c1(exp1, false);
            branch1.push_back(c1);

#ifdef DEBUG
            std::cout << "Will add constraint " << exp1 << " = 1 on branch 2" << std::endl;
            std::cout << "Will add constraint " << exp2 << " = 1 on branch 2" << std::endl;
#endif

            constraint c2(exp1, true);
            constraint c3(exp2, true);

            branch2.push_back(c2);
            branch2.push_back(c3);

            branches.push_back(branch1);
            branches.push_back(branch2);
        } else {
            // If an implication P => Q is false, then
            //  P == true and Q == false.
            std::vector<constraint> curr_branch;
            list *exp1 = exp->content.sublist->at(1);
            list *exp2 = exp->content.sublist->at(2);

#ifdef DEBUG
            std::cout << "Will add constraint " << exp1 << " = 0" << std::endl;
            std::cout << "Will add constraint " << exp2 << " = 1" << std::endl;
#endif

            constraint c1(exp1, true);
            constraint c2(exp2, false);

            curr_branch.push_back(c1);
            curr_branch.push_back(c2);

            branches.push_back(curr_branch);
        }
    } else if(!op.compare("=")) {
        if (v) {
            // If an equivalence P = Q is true, then
            //  either P == Q == true or P == Q == false.
            std::vector<constraint> branch1, branch2;
            list *exp1 = exp->content.sublist->at(1);
            list *exp2 = exp->content.sublist->at(2);

#ifdef DEBUG
            std::cout << "Will add constraint " << exp1 << " = 1 on branch 1" << std::endl;
            std::cout << "Will add constraint " << exp2 << " = 1 on branch 1" << std::endl;
#endif

            constraint c1(exp1, true);
            constraint c2(exp2, true);

            branch1.push_back(c1);
            branch1.push_back(c2);

#ifdef DEBUG
            std::cout << "Will add constraint " << exp1 << " = 0 on branch 2" << std::endl;
            std::cout << "Will add constraint " << exp2 << " = 0 on branch 2" << std::endl;
#endif

            constraint c3(exp1, false);
            constraint c4(exp2, false);

            branch2.push_back(c3);
            branch2.push_back(c4);

            branches.push_back(branch1);
            branches.push_back(branch2);
        } else {
            // If an equivalence P = Q is false, then
            //  either P == false and Q == true or
            //  P == true and Q == false.
            std::vector<constraint> branch1, branch2;
            list *exp1 = exp->content.sublist->at(1);
            list *exp2 = exp->content.sublist->at(2);

#ifdef DEBUG
            std::cout << "Will add constraint " << exp1 << " = 0 on branch 1" << std::endl;
            std::cout << "Will add constraint " << exp2 << " = 1 on branch 1" << std::endl;
#endif

            constraint c1(exp1, false);
            constraint c2(exp2, true);

            branch1.push_back(c1);
            branch1.push_back(c2);

#ifdef DEBUG
            std::cout << "Will add constraint " << exp1 << " = 1 on branch 2" << std::endl;
            std::cout << "Will add constraint " << exp2 << " = 0 on branch 2" << std::endl;
#endif

            constraint c3(exp1, true);
            constraint c4(exp2, false);

            branch2.push_back(c3);
            branch2.push_back(c4);

            branches.push_back(branch1);
            branches.push_back(branch2);
        }
    }

    return branches;
}

solver::solver(spec *base_spec)
{
    m_spec = base_spec;
}

bool solver::check_sat()
{
    branch initial_branch, current_branch;

    // First, we initialize the list of branches
    //  with one branch, containing the assertions
    //  from the spec.
    for(unsigned int i = 0; i < m_spec->constraints().size(); i++) {
        list *curr_exp = m_spec->constraints()[i];
        constraint c(curr_exp, true);

        initial_branch.add_constraint(c);
    }

    m_branches.push_back(initial_branch);

    // Now, we start the main loop.
    while (m_branches.size()) {
        current_branch = m_branches[0];

#ifdef DEBUG
        std::cout << "Will process branch: " << current_branch << std::endl;
#endif

        if(branch_is_closed(current_branch)) {
#ifdef DEBUG
            std::cout << "Branch closed." << std::endl;
#endif
            m_branches.erase(m_branches.begin());
            continue;
        }

        int i_exp_to_branch = get_expression_to_branch(current_branch);
        constraint exp_to_branch = current_branch.constraints[i_exp_to_branch];

        if (i_exp_to_branch == -1) {
            // Branch is open and there is no more
            //  expressions to branch. Problem is sat.
            m_spec->set_sat(true);
            return true;
        }

        // Otherwise, we need to branch.

        std::vector<std::vector<constraint> > new_branches = get_branches(exp_to_branch);

        std::vector<std::vector<constraint> >::const_iterator it;
        for (it = new_branches.begin();
             it != new_branches.end();
             it++) {
            std::vector<constraint> new_constraints = *it;

            branch new_branch = branch(current_branch);

            new_branch.add_constraints(new_constraints);
            new_branch.constraints[i_exp_to_branch].expanded = true;

#ifdef DEBUG
            std::cout << "New branch: " << new_branch << std::endl;
#endif

            m_branches.push_back(new_branch);
        }

        m_branches.erase(m_branches.begin());
    }

    // All branches are closed. Problem is unsat.
    m_spec->set_sat(false);
    return false;
}

model *solver::get_model()
{
#ifdef DEBUG
    std::cout << "Will get model: " << m_branches[0] << std::endl;
#endif
    return new model(m_spec, m_branches[0].atomic_constraints);;
}
