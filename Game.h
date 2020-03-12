
/**
 This class draws a table by scaling and moving blocks.
 The table top is the plane y = 0
*/
#include <vector>
#include <iostream>

using namespace std;

class Game
{    
public:
    Game( int id );
    void toggleTex();
    void toggleTexMode();
    void render(int programID, std::vector< std::vector<int> > gameMap, int xLoc, int zLoc);
};
