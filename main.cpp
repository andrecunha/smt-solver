#include <iostream>
#include <string>
#include "parser.h"
#include "solver.h"
#include "spec.h"
#include "model.h"
#include "data_structures.h"

using namespace std;

int main(int argc, char* argv[])
{
    if  (argc < 2) {
        cout << "usage: " << argv[0] << " file.smt" << endl;
        return 1;
    }

    parser input_parser(argv[1]);
    int result = input_parser.run();

    return result;
}

