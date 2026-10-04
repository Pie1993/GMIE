#ifndef SIMULATION_H
#define SIMULATION_H



#include "gmie.h"
#define NUM_TEST 1


class Simulation
{
    public:
    static void startSimulation(int threads);
    static void updateAreaDimensions(double end);
    static const vector<vector<double>> updatePattern(const unsigned int input_seed);

};

#endif
