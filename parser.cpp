#include <iostream>
#include <fstream>
#include <ctype.h>
#include "parser.h"
#include "data_structures.h"
#include "solver.h"

std::ostream &operator<<(std::ostream &out, const list *obj)
{
    switch (obj->type) {
    case NODE:
        out << *(obj->content.name);
        break;
    case LIST:
        out << "(";

        std::vector<list*>::iterator it;
        for(it = obj->content.sublist->begin();
            it != obj->content.sublist->end();
            ) {
            out << *(it++);
            if (it != obj->content.sublist->end()) {
                out << " ";
            }
        }

        out << ")";
        break;
    }

    return out;
}

parser::parser(std::string filename)
{
    std::ifstream ifs(filename.c_str());
    std::string content((std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()  ) );
    m_file_content = content;
    m_curr_pos = 0;
    m_spec = new spec();
}

list *parser::next_list()
{
    list *curr_list = new list();
    curr_list->type = LIST;
    curr_list->content.sublist = new std::vector<list*>();

    token t = next_token();

    if (t.kind == _NULL) {
        // No more tokens.
        return 0;
    }

    // Expect '('.
    if (t.kind != OPAR) {
        throw parser_exception(
                    "Syntax error: Expected '(', found " + t.image + ".");
    }

    // If it was '(', consume the token.
    consume_token(t);

    do {
        t = next_token();

        switch (t.kind) {
        case NAME:
        {
            list *elem = new list();

            elem->type = NODE;
            elem->content.name = new std::string(t.image);

            curr_list->content.sublist->push_back(elem);

            consume_token(t);
        }
            break;
        case OPAR:
        {
            curr_list->content.sublist->push_back(next_list());
        }
            break;
        case CPAR:
            // This signals the end of the list.
            consume_token(t);
            break;
        case _NULL:
            throw parser_exception("Syntax error: Unexpected end of input. Was expecting ')'.");
            break;
        }
    } while (t.kind != CPAR);

    return curr_list;
}

token parser::next_token()
{
    token t;

    while (isspace(m_file_content[m_curr_pos])) {
        m_curr_pos++;

        if (m_curr_pos >= m_file_content.length()) {
            t.kind = _NULL;
            return t;
        }
    }

    if(m_file_content[m_curr_pos] == '(') {
        t.kind = OPAR;
        t.image = "(";
    } else if (m_file_content[m_curr_pos] == ')') {
        t.kind = CPAR;
        t.image = ")";
    } else {
        unsigned int curr_pos = m_curr_pos;
        while (!isspace(m_file_content[curr_pos])
               && m_file_content[curr_pos] != '('
               && m_file_content[curr_pos] != ')'
               && curr_pos < m_file_content.length()) {

            t.image += m_file_content[curr_pos++];
        }
        t.kind = NAME;
    }

    return t;
}

void parser::consume_token(token t)
{
    m_curr_pos += t.image.length();
}

int parser::print_tokens()
{
    token t;
    do {
        t = next_token();

        if (t.kind != _NULL) {
            std::cout << "<" << t.image << ">" << std::endl;
        }

        consume_token(t);
    } while (t.kind != _NULL);

    return 0;
}

void parser::process_list(list *l)
{
#ifdef DEBUG
    std::cout << "Will process list " << l << std::endl;
#endif
    std::vector<list*> ll = *(l->content.sublist);

    if (ll[0]->type != NODE) {
        throw parser_exception("Syntax error: The first element of a list must be a name.");
    }

    if (!ll[0]->content.name->compare("declare-const")) {
        process_declare_const(l);
    } else if (!ll[0]->content.name->compare("assert")) {
        process_assert(l);
    } else if (!ll[0]->content.name->compare("check-sat")) {
        process_check_sat();
    } else if (!ll[0]->content.name->compare("get-model")) {
        process_get_model();
    } else {
        throw parser_exception("Unknown function " + *(ll[0]->content.name));
    }
}

void parser::process_declare_const(list *l)
{
    std::vector<list*> ll = *(l->content.sublist);
    variable *var;

    if (!ll[2]->content.name->compare("Bool")) {
        var = new bool_variable(*(ll[1]->content.name));
    } else {
        throw parser_exception("Unknown type " + *(ll[2]->content.name));
    }

    m_spec->add_variable(var);
}

void parser::process_assert(list *l)
{
    std::vector<list*> ll = *(l->content.sublist);

    //if (validate_expression(ll[1])) {
        m_spec->add_constraint(ll[1]);
    //}
}

void parser::process_check_sat()
{
#ifdef DEBUG
    std::cout << m_spec << std::endl;
#endif

    solver s(m_spec);
    if (s.check_sat()) {
        std::cout << "sat" << std::endl;
        m_model = s.get_model();
    } else {
        std::cout << "unsat" << std::endl;
        m_model = 0;
    }
}

void parser::process_get_model()
{
    std::cout << m_model << std::endl;
}

int parser::run()
{
    try {
        list *curr_list;
        while ((curr_list = next_list())) {
            process_list(curr_list);
        }
    } catch (parser_exception e) {
        std::cout << e.message << std::endl;
    }

    return 0;
}

