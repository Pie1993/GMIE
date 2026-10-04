#include <iostream>
using namespace std;


#include "OpenGlDem.h"


int main()
{
    OpenGlDem *dem = new OpenGlDem();
    if (dem->getStatus())
        dem->openGlLoopUpdate();
    bool patternChosen = dem->getPatternChosen();
    delete dem;
    dem = 0;

    if(patternChosen)
        Simulation::startSimulation(1);

    return 0;
}
