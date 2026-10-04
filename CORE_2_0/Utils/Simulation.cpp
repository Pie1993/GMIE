#include "Simulation.h"

void Simulation::updateAreaDimensions(double end)
{
    Utils::end = end;
    Utils::total = (Utils::end / Utils::radius);
}

const vector<vector<double>> Simulation::updatePattern(const unsigned int input_seed)
{
    if(Utils::input_seed != input_seed)
    Utils::input_seed = input_seed;
    
    return Utils::generatePattern();
}

void Simulation::startSimulation(int threads)
{

    cout << setprecision(15);

#ifdef VERBOSE

    for (map<int, vector<vector<double>>>::iterator it = Utils::cells.begin(); it != Utils::cells.end(); it++)
    {
        cout << "Key = " << it->first << " | total array = " << it->second.size() << endl;

        for (int i = 0; i < it->second.size(); i++)
        {
            cout << "(";
            for (int j = 0; j < it->second[i].size(); j++)
                cout << it->second[i][j] << " ";
            cout << ")" << endl;
        }
        cout << endl
             << "*******************************" << endl;
    }
#endif

    if (!Utils::canStartSimulation())
    {
        cout << "Cells dimension too little" << endl;
        exit(-1);
    }

    //input parameters
    double *wave_lengths;
    complex<double> *real_and_imaginary_parts;

    UtilsInput::init_wave_lengths(wave_lengths);
    UtilsInput::init_real_and_imaginary_parts(real_and_imaginary_parts);

    bool pol = true;
    double QextMaxSingle = 0.0;
    double QextMaxDouble = 0.0;
    double QextMaxAll = 0.0;
    int nang{3}; // number of scattering angles between 0-180 deg for scattering matrix element S1 S2

    vector<double> Qsca_print(NUM_LINE, 0.0);
    vector<double> Qabs_print(NUM_LINE, 0.0);

    vector<double> Qext_print_single(NUM_LINE, 0.0);
    vector<double> Qext_print_double(NUM_LINE, 0.0);
    vector<double> Qext_print_all(NUM_LINE, 0.0);

    vector<vector<complex<double>>> S1_print(NUM_LINE, vector<complex<double>>(nang, {0.0 + 0.0i}));
    vector<vector<complex<double>>> S2_print(NUM_LINE, vector<complex<double>>(nang, {0.0 + 0.0i}));

    int CHUNK = NUM_LINE / threads;
    double avg_total_time = 0.0;
    for (int i = 0; i < NUM_TEST; i++)
    {
#ifdef OMP
        omp_set_num_threads(threads);
#endif

#ifdef OMP
        clock_t begin = omp_get_wtime();
#else
        clock_t begin = clock();
#endif
        clock_t end = begin;

        vector<vector<double> *> singleParticles;
        map<vector<double> *, vector<vector<double> *>> doubleParticles;
        vector<vector<double> *> keys;

        Utils::check_single_double(singleParticles, doubleParticles, keys, pol);

#pragma omp parallel for schedule(guided)
        for (int j = 0; j < NUM_LINE; j++)
        {
            double wl_0{wave_lengths[j]};               
            complex<double> eper_p{real_and_imaginary_parts[j]};

            double Qsca = 0.0;
            double Qabs = 0.0;
            double Qext_single = 0.0;
            double Qext_double = 0.0;
            double Qext_all = 0.0;
            vector<complex<double>> S1(nang, {0.0 + 0.0i});
            vector<complex<double>> S2(nang, {0.0 + 0.0i});

            tie(Qsca, Qabs, Qext_single, Qext_double, S1, S2) = 
            gmie(singleParticles, doubleParticles, keys, wl_0, nang, eper_p, pol);
            Qext_all = Qext_single + Qext_double;

            Qsca_print[j] = Qsca;
            Qabs_print[j] = Qabs;
            //++++++++++++
            Qext_print_single[j] = Qext_single;
            Qext_print_double[j] = Qext_double;
            Qext_print_all[j] = Qext_all;
            //++++++++++++
            S1_print[j] = S1;
            S2_print[j] = S2;
        }

#ifdef OMP
        end = omp_get_wtime();
        double elapsedTime = (end - begin);
#else
        end = clock();
        double elapsedTime = (double)(end - begin) / CLOCKS_PER_SEC;
#endif

        for (int i = 0; i < NUM_LINE; i++)
        {
            if (Qext_print_double[i] > QextMaxDouble)
                QextMaxDouble = Qext_print_double[i];

            if (Qext_print_single[i] > QextMaxSingle)
                QextMaxSingle = Qext_print_single[i];

            if (Qext_print_all[i] > QextMaxAll)
                QextMaxAll = Qext_print_all[i];
        }

        for (int i = 0; i < NUM_LINE; i++)
        {
            Qext_print_single[i] = Qext_print_single[i] / QextMaxSingle;
            Qext_print_double[i] = Qext_print_double[i] / QextMaxDouble;
            Qext_print_all[i] = Qext_print_all[i] / QextMaxAll;
        }

        avg_total_time += elapsedTime;
    }
    
    avg_total_time /= NUM_TEST;
    cout << "AVG Elapsed time " << avg_total_time << endl;
    Utils::exportResultToFile(Qsca_print, Qabs_print, Qext_print_single, Qext_print_double, Qext_print_all, S1_print, S2_print, nang, threads, avg_total_time);

    delete[] wave_lengths;
    delete[] real_and_imaginary_parts;
}
