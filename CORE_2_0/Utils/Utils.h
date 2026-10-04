#ifndef UTILS_H
#define UTILS_H
#include <iostream>
using namespace std;
#include <iomanip>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <map>
#include <tuple>
#include <complex>
#include <limits>
#include "UtilsInput.h"
#include <omp.h>

// #define VERBOSE

#define CHECK
#define RANDOM_VERSION
#define CONSTRAINTS

#define ROWS 10 //20
#define COLUMNS 10 //50
#define CELLS  ROWS * COLUMNS

#define NEIGHBORS 8
#define LEFT -1
#define RIGHT +1
#define UP +COLUMNS
#define DOWN -COLUMNS
#define UP_LEFT UP + LEFT
#define UP_RIGHT UP + RIGHT
#define DOWN_LEFT DOWN + LEFT
#define DOWN_RIGHT DOWN + RIGHT

#define RANDOM_FACTOR  10000 // almeno 10000 --> aumentare per aumentare la precisione
#define ROW_DIVIDER ((radius + end) / ROWS)
#define COL_DIVIDER ((radius + end) / COLUMNS)
#define RATE_RANDOM_FACTOR  (RANDOM_FACTOR / (RANDOM_FACTOR * (double)RANDOM_FACTOR))
#define DEFAULT -1

using namespace std;

class Utils
{
public:
static const vector<vector<double> >  generatePattern();
static void applyConstraints();
static void exportToFile(const vector<vector<double> > & );
static string computeVertex(const string & ,const vector<double> &);
static void withNeighbors(int key);
static void withinCell(vector<vector<double>> &cell);
static void getNeighbors(int key, int *neighbors,int & numNeighbors);
static void getNeighborsForComputation(int key, int *neighbors,int & numNeighbors);
static bool checkNeighbors(int key, int newKey);
static bool checkDistanceConstraints(double x1, double x2, double y1, double y2);
static bool checkDistanceBetweenParticles(double x1, double x2, double y1, double y2,bool pol);
static void exportResultToFile(vector<double> &Qsca,vector<double> &Qabs,vector<double>&Qext_print_single, vector<double> &Qext_print_double,vector<double>&Qext_print_all, const vector<vector<complex<double>>> &S1, const vector<vector<complex<double>>> &S2,int nang,int threads,double elapse_time);
static bool canStartSimulation();
static void check_single_double(vector<vector<double>*> &singleParticles,map<vector<double>*,vector<vector<double>*>> &doubleParticles,vector<vector<double>*> &keys,bool pol);
static vector<vector<double>> getArrayPositions();

static string templateSphere;
static string templateFolder;
static string outputFolder;
static string outputSphere;
static string resultFile;
static unsigned int input_seed;
static double end;
static int start;

static int total;
static double diameter;
static double radius;
static double minDistance;
static double rateRandomFactor;
static int randomFactor;
static double minDistanceBetweenParticles;
static double maxDistanceBetweenParticles;


static map<int,vector<vector<double>>>cells;
static vector<vector<double>> positions;

};

#endif
