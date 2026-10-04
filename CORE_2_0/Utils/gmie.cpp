
#include "gmie.h"

const complex<double> eper_m{1.0 + 0.0i};          //complex permittivity of medium ε
const complex<double> mper_m{1.0 + 0.0i};          //complex permiability of medium μ
const complex<double> m_m = sqrt(eper_m * mper_m); //refractive index of medium
const complex<double> mper_p{1.0 + 0.0i};          //complex permiability of particle μ1
const double r_p{0.02};                            // particle radius

tuple<double, double, double, double, vector<complex<double>>, vector<complex<double>>>
gmie(const vector<vector<double> *> &singleParticles, map<vector<double> *,
 vector<vector<double> *>> &doubleParticles, const vector<vector<double>*> &keys,
 const double &wl_0, const int &nang, const complex<double> &eper_p, const bool &pol){

    complex<double> m_p = sqrt(eper_p * mper_p); 
    complex<double> m_r = m_p / m_m;             
    double k0 = 2 * M_PI / wl_0;                 
    complex<double> k = m_m * k0;                

    vector<complex<double>> S1(nang, {0.0 + 0.0i}), S2(nang, {0.0 + 0.0i});
    double Qsca{0.0}, Qext_single{0.0}, Qext_double{0.0}, Qabs{0.0};
    map<int, vector<vector<double>>> &cells = Utils::cells;


    for(int i=0;i<keys.size();i++)
        for (int j = 0; j < doubleParticles[keys[i]].size(); j++)
            double_particle(keys[i][0][0], keys[i][0][1], doubleParticles[keys[i]][j][0][0], 
            doubleParticles[keys[i]][j][0][1], k,m_r, mper_p, mper_m, nang, Qsca, Qext_double, 
            Qabs, S1, S2, pol);


        for (int i = 0; i < singleParticles.size(); i++)
            single_particle(singleParticles[i][0][0], singleParticles[i][0][1], k, m_r, mper_p, 
            mper_m, nang, Qsca, Qext_single, Qabs, S1, S2);
    

    return forward_as_tuple(Qsca, Qabs, Qext_single, Qext_double, S1, S2);
}

void single_particle(const double &xPos, const double &yPos, const complex<double> &k, const complex<double> &m_r,
                     const complex<double> &mper_p, const complex<double> &mper_m, const int &nang, double &QscaTmp, double &QextTmp, double &QabsTmp,
                     vector<complex<double>> &S1Tmp, vector<complex<double>> &S2Tmp)
{

    complex<double> x = xPos * k * r_p;
    complex<double> y = yPos * k * m_r * r_p;

    int nstop = floor(abs(x) + 4 * cbrt(abs(x)) + 2);          //number of expansion terms for partial wave coefficients (BH83)
    int nmx = max(nstop, static_cast<int>(ceil(abs(y)))) + 15; //number of terms for downward recurence calculations

    //---------------Logarithmic derivative DD calculated by downward recurrence-------------------
    // beginning with initial value (0.,0.) at nmx
    // y:=x*m_r   argument of DD
    vector<complex<double>> DD(nmx, {0.0 + 0.0i});
    for (int n = nmx - 1; n > 1; --n)
        DD[n - 2] = (1.0 * n) / y - 1.0 / (DD[n - 1] + (1.0 * n) / y);

    //-----------Reccati-Bessel function PSI(0:nstop) calculated by downward recurrence----------
    //Reference: Mischenko et al. 2002, Scattering, Absorption and Emission of Light by Small Particles 3rd, pp.167-169
    // x:=k*r_p argument of PSI
    //RR=0.0d0; ! R(n):=PSI(n)/PSI(n-1)
    vector<complex<double>> RR(nmx + 1, {0.0 + 0.0i});
    RR[nmx] = x / (2.0 * nmx + 1.0); //starting value of downward recurrence
    for (int n = nmx - 1; n > -1; --n)
        RR[n] = 1.0 / ((2.0 * n + 1.0) / x - RR[n + 1]); // R(n) := Rn

    vector<complex<double>> psi(nmx + 1, {0.0 + 0.0i});
    psi[0] = RR[0] * cos(x);
    for (int n = 1; n <= nstop; ++n)
        psi[n] = RR[n] * psi[n - 1]; // PSI(n) := PSIn

    // -------Reccati-Bessel function chi(0:nstop) calculated by upward recurrence------
    // beginning with initial values chi(-1)=sin(x), chi(0)=-cos(x)
    // Reference: Bphren and Huffman 1983 (Appendix A, p.478)
    // CHI(x):= x*y(x) where y(x) is spherical bessel function of second kind
    // This is contrast to the definition of BH83: CHI(x):= -x*y(x)
    // x:=k*r_p argument of CHI
    vector<complex<double>> chi(nstop + 1, {0.0 + 0.0i});
    chi[0] = -cos(x);
    chi[1] = (1.0 / x) * chi[0] - sin(x);
    for (int n = 2; n <= nstop; ++n)
        chi[n] = ((2.0 * n - 1.0) / x) * chi[n - 1] - chi[n - 2];

    vector<complex<double>> qsi(nstop + 1, {0.0 + 0.0i});
    for (int n = 0; n <= nstop; ++n)
        qsi[n] = psi[n] + chi[n] * complex<double>(0.0 + 1.0i);

    //-----------Reccati-Bessel function PSIM(0:nstop) calculated by downward recurrence----------
    // Reference: Mischenko et al. 2002, Scattering, Absorption and Emission of Light by Small Particles 3rd, pp.167-169
    // y:=x*m_r argument of PSI
    fill(RR.begin(), RR.end(), 0.0 + 0.0i);
    RR[nmx] = y / (2.0 * nmx + 1.0);
    for (int n = nmx - 1; n > -1; --n)
        RR[n] = 1.0 / ((2.0 * n + 1.0) / y - RR[n + 1]); // R(n) := Rn

    vector<complex<double>> psim(nstop + 1, {0.0 + 0.0i});
    psim[0] = RR[0] * cos(y);
    for (int n = 1; n <= nstop; ++n)
        psim[n] = RR[n] * psim[n - 1];

    //----------Evaluations of partial wave coefficients a and b defined by BH83, Eqs.4.56-4.57---------------------------------------
    vector<complex<double>> a(nstop, {0.0 + 0.0i}), b(nstop, {0.0 + 0.0i}), c(nstop, {0.0 + 0.0i}), d(nstop, {0.0 + 0.0i});
    for (int n = 0; n < nstop; ++n)
    {
        a[n] = ((mper_p / (mper_m * m_r) * DD[n] + (n + 1.0) / x) * psi[n + 1] - psi[n]) / ((mper_p / (mper_m * m_r) * DD[n] + (n + 1.0) / x) * qsi[n + 1] - qsi[n]); // BH83, Eq.4.88
        b[n] = ((mper_m * m_r / mper_p * DD[n] + (n + 1.0) / x) * psi[n + 1] - psi[n]) / ((mper_m * m_r / mper_p * DD[n] + (n + 1.0) / x) * qsi[n + 1] - qsi[n]);     // BH83, Eq.4.88
        c[n] = complex<double>(0.0 + 1.0i) * m_r / (psim[n + 1] * (qsi[n] - qsi[n + 1] * (mper_m * m_r / mper_p * DD[n] + (n + 1.0) / x)));
        d[n] = complex<double>(0.0 + 1.0i) * mper_p / mper_m / (psim[n + 1] * (qsi[n] - qsi[n + 1] * (mper_p / (mper_m * m_r) * DD[n] + (n + 1.0) / x)));
    }

    vector<double> fn1(nstop, 0.0), fn2(nstop, 0.0);
    for (int n = 0; n < nstop; ++n)
    {
        fn1[n] = 2.0 * (n + 1) + 1.0;
        fn2[n] = fn1[n] / ((n + 1.0) * (n + 2.0));
    }

    vector<complex<double>> S1(nang, {0.0 + 0.0i}), S2(nang, {0.0 + 0.0i});
    vector<double> pie(nstop, {0.0}), tau(nstop, {0.0});
    for (int j = 0; j < nang; ++j)
    {
        double dtheta{M_PI / (nang - 1)};
        double theta = j * dtheta;
        double mu = cos(theta);
        pie[0] = 1.0;
        pie[1] = 3 * mu * pie[0];
        tau[0] = mu * pie[0];
        tau[1] = 2 * mu * pie[1] - 3 * pie[0];
        for (int n = 2; n < nstop; ++n)
        {
            pie[n] = ((2.0 * (n + 1.0) - 1.0) / n) * mu * pie[n - 1] - ((n + 1.0) / n) * pie[n - 2];
            tau[n] = (n + 1.0) * mu * pie[n] - (n + 2.0) * pie[n - 1];
        }
        for (int n = 0; n < nstop; ++n)
        {
            S1[j] += fn2[n] * (a[n] * pie[n] + b[n] * tau[n]);
            S2[j] += fn2[n] * (a[n] * tau[n] + b[n] * pie[n]);
        }
    }

    double Qsca{0.0}, Qext{0.0}, Qabs{0.0};
    if (imag(x) == 0)
    { // nonabsorbing media
        //------- Following Qsca,Qext,Qabs,S1,S2 are defined only for nonabsorbing media (Imag(m_m)=0)-------
        for (int n = 0; n < nstop; ++n)
        {
            Qsca = Qsca + fn1[n] * (abs(a[n]) * abs(a[n]) + abs(b[n]) * abs(b[n])); // BH83, Eq.4.61
            Qext = Qext + fn1[n] * real(a[n] + b[n]);
        }
        Qsca *= 2.0 / real(x) / real(x);
        Qext *= 2.0 / real(x) / real(x) * 0.000001;
        Qabs = Qext - Qsca;
    }
    else
    { // absobing media
        Qsca = Qabs = numeric_limits<double>::quiet_NaN();
        //-------Extinction efficiency based on the optical theorem Qext_opt is defined also for absorbing media--------
        //extinction efficiency calculated from the optical theorem
        Qext = 4.0 / (r_p * r_p * real(k)) * imag(S1[0] * complex<double>(0.0 + 1.0i) / real(k)); // MISH07 Eq.87
        //--------------------------------------------------------------------------------------------------------------
        for (int i = 0; i < nang; ++i)
        {
            S1[i] = {numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN()};
            S2[i] = {numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN()};
        }
    }

    // ///-------------------------
    // std::cout << QscaTmp << "   " << Qsca << endl;
    // std::cout << QabsTmp << "   " << Qabs << endl;
    // std::cout << QextTmp << "   " << Qext << endl;
    // cout<<"**************"<<endl;

    QscaTmp += Qsca;
    QabsTmp += Qabs;
    QextTmp += Qext;

    for (int i = 0; i < nang; ++i)
    {
        S1Tmp[i] += S1[i];
        S2Tmp[i] += S2[i];
    }
}
void double_particle(const double &xPos, const double &yPos, const double &xPos2, const double &yPos2, const complex<double> &k, const complex<double> &m_r,
                     const complex<double> &mper_p, const complex<double> &mper_m, const int &nang, double &QscaTmp, double &QextTmp, double &QabsTmp,
                     vector<complex<double>> &S1Tmp, vector<complex<double>> &S2Tmp, const bool &pol)
{

    complex<double> x = k * r_p;
    complex<double> y = k * m_r * r_p;

    double distance = 0.0;

    if (pol)
        distance = -abs(xPos2 - xPos);
    else
        distance = -abs(yPos2 - yPos);

    distance = distance - (2 * r_p);
    distance = distance / (2 * r_p);

    double distancePointToPoint = pow(M_E, (0.18 * (-distance)) / 0.23);

    int nstop = floor(abs(x) + 4 * cbrt(abs(x)) + 2);          //number of expansion terms for partial wave coefficients (BH83)
    int nmx = max(nstop, static_cast<int>(ceil(abs(y)))) + 15; //number of terms for downward recurence calculations

    //---------------Logarithmic derivative DD calculated by downward recurrence-------------------
    // beginning with initial value (0.,0.) at nmx
    // y:=x*m_r   argument of DD
    vector<complex<double>> DD(nmx, {0.0 + 0.0i});
    for (int n = nmx - 1; n > 1; --n)
        DD[n - 2] = distancePointToPoint * (1.0 * n) / y - 1.0 / (DD[n - 1] + (1.0 * n) / y);

    //-----------Reccati-Bessel function PSI(0:nstop) calculated by downward recurrence----------
    //Reference: Mischenko et al. 2002, Scattering, Absorption and Emission of Light by Small Particles 3rd, pp.167-169
    // x:=k*r_p argument of PSI
    //RR=0.0d0; ! R(n):=PSI(n)/PSI(n-1)
    vector<complex<double>> RR(nmx + 1, {0.0 + 0.0i});
    RR[nmx] = x / (2.0 * nmx + 1.0); //starting value of downward recurrence
    for (int n = nmx - 1; n > -1; --n)
        RR[n] = 1.0 / ((2.0 * n + 1.0) / x - RR[n + 1]); // R(n) := Rn

    vector<complex<double>> psi(nmx + 1, {0.0 + 0.0i});
    psi[0] = RR[0] * cos(x);
    for (int n = 1; n <= nstop; ++n)
        psi[n] = distancePointToPoint * RR[n] * psi[n - 1]; // PSI(n) := PSIn

    // -------Reccati-Bessel function chi(0:nstop) calculated by upward recurrence------
    // beginning with initial values chi(-1)=sin(x), chi(0)=-cos(x)
    // Reference: Bphren and Huffman 1983 (Appendix A, p.478)
    // CHI(x):= x*y(x) where y(x) is spherical bessel function of second kind
    // This is contrast to the definition of BH83: CHI(x):= -x*y(x)
    // x:=k*r_p argument of CHI
    vector<complex<double>> chi(nstop + 1, {0.0 + 0.0i});
    chi[0] = -cos(x);
    chi[1] = (1.0 / x) * chi[0] - sin(x);
    for (int n = 2; n <= nstop; ++n)
        chi[n] = ((2.0 * n - 1.0) / x) * chi[n - 1] - chi[n - 2];

    vector<complex<double>> qsi(nstop + 1, {0.0 + 0.0i});
    for (int n = 0; n <= nstop; ++n)
        qsi[n] = psi[n] + chi[n] * complex<double>(0.0 + 1.0i);

    //-----------Reccati-Bessel function PSIM(0:nstop) calculated by downward recurrence----------
    // Reference: Mischenko et al. 2002, Scattering, Absorption and Emission of Light by Small Particles 3rd, pp.167-169
    // y:=x*m_r argument of PSI
    fill(RR.begin(), RR.end(), 0.0 + 0.0i);
    RR[nmx] = y / (2.0 * nmx + 1.0);
    for (int n = nmx - 1; n > -1; --n)
        RR[n] = 1.0 / ((2.0 * n + 1.0) / y - RR[n + 1]); // R(n) := Rn

    vector<complex<double>> psim(nstop + 1, {0.0 + 0.0i});
    psim[0] = RR[0] * cos(y);
    for (int n = 1; n <= nstop; ++n)
        psim[n] = distancePointToPoint * RR[n] * psim[n - 1];

    //----------Evaluations of partial wave coefficients a and b defined by BH83, Eqs.4.56-4.57---------------------------------------
    vector<complex<double>> a(nstop, {0.0 + 0.0i}), b(nstop, {0.0 + 0.0i}), c(nstop, {0.0 + 0.0i}), d(nstop, {0.0 + 0.0i});
    for (int n = 0; n < nstop; ++n)
    {
        a[n] = ((mper_p / (mper_m * m_r) * DD[n] + (n + 1.0) / x) * psi[n + 1] - psi[n]) / ((mper_p / (mper_m * m_r) * DD[n] + (n + 1.0) / x) * qsi[n + 1] - qsi[n]); // BH83, Eq.4.88
        b[n] = ((mper_m * m_r / mper_p * DD[n] + (n + 1.0) / x) * psi[n + 1] - psi[n]) / ((mper_m * m_r / mper_p * DD[n] + (n + 1.0) / x) * qsi[n + 1] - qsi[n]);     // BH83, Eq.4.88
        c[n] = complex<double>(0.0 + 1.0i) * m_r / (psim[n + 1] * (qsi[n] - qsi[n + 1] * (mper_m * m_r / mper_p * DD[n] + (n + 1.0) / x)));
        d[n] = complex<double>(0.0 + 1.0i) * mper_p / mper_m / (psim[n + 1] * (qsi[n] - qsi[n + 1] * (mper_p / (mper_m * m_r) * DD[n] + (n + 1.0) / x)));
    }

    vector<double> fn1(nstop, 0.0), fn2(nstop, 0.0);
    for (int n = 0; n < nstop; ++n)
    {
        fn1[n] = 2.0 * (n + 1) + 1.0;
        fn2[n] = fn1[n] / ((n + 1.0) * (n + 2.0));
    }

    vector<complex<double>> S1(nang, {0.0 + 0.0i}), S2(nang, {0.0 + 0.0i});
    vector<double> pie(nstop, {0.0}), tau(nstop, {0.0});
    for (int j = 0; j < nang; ++j)
    {
        double dtheta{M_PI / (nang - 1)};
        double theta = j * dtheta;
        double mu = cos(theta);
        pie[0] = 1.0;
        pie[1] = 3 * mu * pie[0];
        tau[0] = mu * pie[0];
        tau[1] = 2 * mu * pie[1] - 3 * pie[0];
        for (int n = 2; n < nstop; ++n)
        {
            pie[n] = ((2.0 * (n + 1.0) - 1.0) / n) * mu * pie[n - 1] - ((n + 1.0) / n) * pie[n - 2];
            tau[n] = (n + 1.0) * mu * pie[n] - (n + 2.0) * pie[n - 1];
        }
        for (int n = 0; n < nstop; ++n)
        {
            S1[j] += fn2[n] * (a[n] * pie[n] + b[n] * tau[n]);
            S2[j] += fn2[n] * (a[n] * tau[n] + b[n] * pie[n]);
        }
    }

    double Qsca{0.0}, Qext{0.0}, Qabs{0.0};
    if (imag(x) == 0)
    { // nonabsorbing media
        //------- Following Qsca,Qext,Qabs,S1,S2 are defined only for nonabsorbing media (Imag(m_m)=0)-------
        for (int n = 0; n < nstop; ++n)
        {
            Qsca = Qsca + fn1[n] * (abs(a[n]) * abs(a[n]) + abs(b[n]) * abs(b[n])); // BH83, Eq.4.61
            Qext = Qext + fn1[n] * real(a[n] + b[n]);
        }
        Qsca *= 2.0 / real(x) / real(x);
        Qext *= 2.0 / real(x) / real(x) * pow(10, -12);
        Qabs = Qext - Qsca;
    }
    else
    { // absobing media
        Qsca = Qabs = numeric_limits<double>::quiet_NaN();
        //-------Extinction efficiency based on the optical theorem Qext_opt is defined also for absorbing media--------
        //extinction efficiency calculated from the optical theorem
        Qext = 4.0 / (r_p * r_p * real(k)) * imag(S1[0] * complex<double>(0.0 + 1.0i) / real(k)); // MISH07 Eq.87
        //--------------------------------------------------------------------------------------------------------------
        for (int i = 0; i < nang; ++i)
        {
            S1[i] = {numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN()};
            S2[i] = {numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN()};
        }
    }

    QscaTmp += Qsca;
    QabsTmp += Qabs;
    QextTmp += Qext;

    for (int i = 0; i < nang; ++i)
    {
        S1Tmp[i] += S1[i];
        S2Tmp[i] += S2[i];
    }
}
