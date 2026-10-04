#ifndef GMIE_H
#define GMIE_H

#include "Utils.h"

#pragma omp declare reduction(vec_complex_double : std::vector<complex<double>> : \
                              std::transform(omp_out.begin(), omp_out.end(), omp_in.begin(), omp_out.begin(), std::plus<complex<double>>())) \
                    initializer(omp_priv = decltype(omp_orig)(omp_orig.size()))

tuple<double,double,double,double,vector<complex<double>>,vector<complex<double>>>
gmie(const vector<vector<double>*> &singleParticles,map<vector<double>*,vector<vector<double>*>> &doubleParticles,const vector<vector<double>*> &keys,const double& wl_0, const int& nang,
const complex<double>& eper_p,const bool &pol);
void single_particle(const double &xPos,const double &yPos,const complex<double>&k,const complex<double>& m_r,
const complex<double> &mper_p,const complex<double> &mper_m, const int &nang, double &QscaTmp, double &Qext_single, double &QabsTmp,
    vector<complex<double>> &S1Tmp, vector<complex<double>> &S2Tmp);
void double_particle(const double &xPos,const double &yPos,const double &xPos1,const double &yPos1,const complex<double>&k, const complex<double>& m_r,
const complex<double> &mper_p,const complex<double> &mper_m, const int &nang, double &QscaTmp, double &Qext_double, double &QabsTmp,
    vector<complex<double>> &S1Tmp, vector<complex<double>> &S2Tmp,const bool &pol);




#endif