#ifndef UTILSINPUT_H
#define UTILSINPUT_H
#include <iostream>
using namespace std;
#include <complex>

#define NUM_LINE 201

class UtilsInput
{

public:
static void init_wave_lengths(double *&wave_lengths);
static void init_real_and_imaginary_parts(complex<double> *&real_and_imaginary_parts);


};

#endif