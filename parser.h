#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "spec.h"
#include "model.h"

/* Defines the token returned by the lexical analyzer. */

enum kind_t {OPAR, CPAR, NAME, _NULL};
typedef enum kind_t kind_t;

class token
{
public:
    kind_t kind;
    std::string image;
};

// Processes the input SMT file and runs the statements.
class parser
{
public:
    // Creates a parser that will read from a file.
    parser(std::string filename);

    int print_tokens();

    // Runs the commands in the file.
    int run();
private:
    std::string m_file_content;
    spec *m_spec;
    model *m_model;
    unsigned int m_curr_pos;

    list *next_list();
    token next_token();
    void consume_token(token t);
    void process_list(list *l);

    void process_declare_const(list *l);
    void process_assert(list *l);
    void process_check_sat();
    void process_get_model();
};

// Represents a parsing error (either lexical, syntactic, or semantic).
class parser_exception
{
public:
    parser_exception(std::string m) : message(m){}
    std::string message;
};

#endif // PARSER_H
