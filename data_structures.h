#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <map>

// The types supported by this solver.
enum type_t {bool_t};
typedef enum type_t type_t;

// Strings containing the name of each type.
//  Used for printing.
extern std::string type_names[1];

// Represents a generic variable. Each variable
//  has a name and a type.
class variable
{
public:
    virtual std::string name() const = 0;
    virtual type_t type() const = 0;

    bool has_value_set;

    friend std::ostream &operator<<(std::ostream &out, const variable *obj);
};

// Represents a boolean variable.
class bool_variable : public variable
{
private:
    bool m_value;
    std::string m_name;

public:
    // Constructs a variable with a name, but no value.
    bool_variable(std::string name);
    // Constructs a variable with a name and a value.
    bool_variable(std::string name, bool value);

    std::string name() const;
    type_t type() const;

    // Getter and setter for m_value.
    bool value();
    void set_value(bool value);

    // The two logic constants 1 and 0.
    static const bool_variable *TRUE;
    static const bool_variable *FALSE;

    friend std::ostream &operator<<(std::ostream &out, const variable *obj);
};

/* Defines the list that a parser returns. */

enum list_type_t {NODE, LIST};
typedef enum list_type_t list_type_t;

class list {
public:
    list_type_t type;
    union content {
        std::string *name;
        std::vector<list*> *sublist;
    } content;

    friend std::ostream &operator<<(std::ostream &out, const list *obj);
};

// A constraint is a pair (expression, value), that
//  imposes that the expression must evaluate to the
//  indicated value. Por example, ((=> P Q), true) means
//  that it must be true that P => Q.
class constraint
{
public:
    list *expression;
    bool value;

    constraint(list *exp, bool v);

    // The number of branches that this expression will
    //  generate if expanded. Used to minimize the number
    //  of branches in the tableaux.
    int required_branches;

    // Keeps track of what expressions have already been
    //  expanded in the tableaux.
    bool expanded;

    friend std::ostream &operator<<(std::ostream &out, const constraint obj);
};

#endif // DATA_STRUCTURES_H
