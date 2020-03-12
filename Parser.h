#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>

class parser{
    public:
        parser();
        std::vector< std::vector<int> > parseInput(int argc, char**argv);
        void printMatrix(std::vector< std::vector<int> > gameMap);
        ~parser();
    private:
};

#endif // PARSER_H
