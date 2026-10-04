
#include "Utils/Simulation.h"

#define SERIAL 1
#define THREADS 4

int main(int argc, char *argv[])
{

    int seed = DEFAULT;

#ifdef OMP
    omp_set_num_threads(THREADS);
#endif

    if (argc > 1)
    {

        std::istringstream ss(argv[1]);

        if (!(ss >> seed))
        {
            std::cerr << "Invalid number: " << argv[1] << '\n';
            exit(-1);
        }
        else if (!ss.eof())
        {
            std::cerr << "Trailing characters after number: " << argv[1] << '\n';
            exit(-1);
        }
    }

    //cout << seed << endl;
    double dimensions[]{0.000003, 0.000005, 0.000008, 0.00001, 0.000015, 0.00002, 0.000025, 0.00003, 0.000035, 0.00004};

    for (int i = 0; i < 10; i++)
    {
        cout << "**********************  " << to_string(dimensions[i]) << "  **********************" << endl;
        Simulation::updateAreaDimensions(dimensions[i]);
        const vector<vector<double>> positions = Simulation::updatePattern(seed);
        for (int j = THREADS; j <= THREADS; j *= 2)
        {
            cout << "**********************  "
                 << "THREADS  " << to_string(j) << "  **********************" << endl;
            Simulation::startSimulation(j);
        }
    }

    // for (int i = 0; i < positions.size(); i++)
    // {
    //     for (int j = 0; j < positions[i].size(); j++)
    //         cout << positions[i][j] << "  ";
    //     cout << endl;
    // }
}

