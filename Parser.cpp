#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include "Parser.h"

// Constructor
parser::parser(){

}
// Parse the input file into a 2d vector.
std::vector< std::vector<int> > parser::parseInput(int argc, char**argv){
    // Declare variables.
    std::vector < std::vector<int> > gameMap;
    std::vector <int> tempVec;
    std::string str = "";
    int i = 0;
    // Read in the first line containing the matrix size.
    std::ifstream infile("ground.txt"); // Open the file.
    // Reads in all the numbers and create a vector of vectors.
    while ( getline(infile, str) ){
        if(str.empty()) break;
        for ( i = 0 ; i < str.size() ; i++ ) {
            switch(str[i]){
                case '*':
                    tempVec.push_back(1);
                    break;
                case 'X':
                    tempVec.push_back(2);
                    break;
                default:
                    tempVec.push_back(0);
            }
        }
        gameMap.push_back(tempVec);
        tempVec.clear();
    }
    return gameMap;
}
// Print parsed input vector.
void parser::printMatrix(std::vector< std::vector<int> > gameMap){
    std::cout << "Print Matrix" << std::endl; 
    std::vector<int> tempVec;
    for(int i = 0; i < gameMap.size(); i++ ){
        tempVec = gameMap[i];
        for (int k = 0; k < tempVec.size(); k++)
        {
            std::cout << tempVec[k];
        }
        std::cout << std::endl;
        tempVec.clear();
    }
}
// Deconstructor
parser::~parser(){

}
