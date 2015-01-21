CC=g++

all: debug

debug: data_structures.cpp main.cpp model.cpp parser.cpp solver.cpp spec.cpp
	$(CC) -DDEBUG -o smt_solver data_structures.cpp main.cpp model.cpp parser.cpp solver.cpp spec.cpp

release: data_structures.cpp main.cpp model.cpp parser.cpp solver.cpp spec.cpp
	$(CC) -o smt_solver data_structures.cpp main.cpp model.cpp parser.cpp solver.cpp spec.cpp
