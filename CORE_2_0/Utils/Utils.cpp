#include "Utils.h"
string Utils::templateFolder = "./templateFile/";
string Utils::templateSphere = templateFolder + "sphere.stl";
string Utils::outputFolder = "./outputFile/";
string Utils::outputSphere = outputFolder + "sphere.stl";
string Utils::resultFile = outputFolder + "result";

unsigned int Utils::input_seed = DEFAULT;
double Utils::end = 0.000003;
double Utils::diameter = 0.00000004;
double Utils::radius = diameter / 2.0;
double Utils::minDistance = radius * 2.1;
double Utils::minDistanceBetweenParticles = radius * 2.1;
double Utils::maxDistanceBetweenParticles = radius * 3.0;
int Utils::total = (end / radius);
int Utils::start = 0;

map<int, vector<vector<double>>> Utils::cells;
vector<vector<double>> Utils::positions;

const vector<vector<double>> Utils::generatePattern()
{
    cells.clear();

    unsigned int seed = 0;

#ifdef RANDOM_VERSION
        seed = time(0);
#endif


if(input_seed != -1)
    seed = input_seed;

    srand(seed);


    cout << "seed     " << seed << endl;
    for (int i = start; i < total; i++)
    {
        for (int j = start; j < total; j++)
        {
            int randX = rand_r(&seed) % RANDOM_FACTOR;
            int randY = rand_r(&seed) % RANDOM_FACTOR;
            // radius was added to avoid coordination to fall in 0.0 value
            double x = radius + randX * RATE_RANDOM_FACTOR * end; // randX * RATE_RANDOM_FACTOR  ;
            double y = radius + randY * RATE_RANDOM_FACTOR * end; // randY * RATE_RANDOM_FACTOR  ;
            double z = 0.0;
            int keyX = x / COL_DIVIDER;
            int keyY = y / ROW_DIVIDER;
            int key = keyX + keyY * ROWS;
            vector<double> v = vector<double>();
            v.push_back(x);
            v.push_back(y);
            v.push_back(z);
            cells[key].push_back(v);
        }
    }

    cout << setprecision(15);
#ifdef VERBOSE
    for (int i = 0; i < positions.size(); i++)
    {
        for (int j = 0; j < 3; j++)
            cout << positions[i][j] << " ";
        cout << endl;
    }
#endif

#ifdef CONSTRAINTS
    applyConstraints();
#endif

    positions.clear();
    positions = getArrayPositions();
    cout << "Total amount of particles = " << positions.size() << endl;

#ifdef CHECK
    int count = 0;
#pragma omp parallel for reduction(+ \
                                   : count) collapse(2)
    for (int i = 0; i < positions.size(); i++)
        for (int j = 0; j < positions.size(); j++)
            if (i != j && checkDistanceConstraints(positions[i][0], positions[j][0], positions[i][1], positions[j][1]))
                count++;
 //   cout << "minore  " << count << endl;
#endif
    return positions;
}

vector<vector<double>> Utils::getArrayPositions()
{
    vector<vector<double>> positions = vector<vector<double>>();
    for (int key = 0; key < CELLS; key++)
        for (int i = 0; i < cells[key].size(); i++)
            positions.push_back(cells[key][i]);
    return positions;
}

void Utils::applyConstraints()
{
    for (map<int, vector<vector<double>>>::iterator it = cells.begin(); it != cells.end(); it++)
        withinCell(it->second);

    for (map<int, vector<vector<double>>>::iterator it = cells.begin(); it != cells.end(); it++)
        withNeighbors(it->first);
}

void Utils::withinCell(vector<vector<double>> &cell)
{
    for (vector<vector<double>>::iterator it = cell.begin(); it != cell.end(); it++)
    {
        vector<vector<double>>::iterator it2 = it + 1;
        while (it2 != cell.end())
        {
            if (
                checkDistanceConstraints(
                    (*it)[0],  //x1
                    (*it2)[0], //x2
                    (*it)[1],  //y1
                    (*it2)[1]) //y2
            )
                it2 = cell.erase(it2);
            else
                it2++;
        }
    }
}

bool Utils::checkDistanceConstraints(double x1, double x2, double y1, double y2)
{
    double distance = abs(x1 - x2) + abs(y1 - y2);
    if (distance < minDistance)
        return true;

    return false;
}

bool Utils::checkDistanceBetweenParticles(double x1, double x2, double y1, double y2, bool pol)
{
    double distance = 0.0;
    if (pol)
        distance = abs(x1 - x2);
    else
        distance = abs(y1 - y2);

    if (distance >= minDistanceBetweenParticles && distance <= maxDistanceBetweenParticles)
        return true;
    return false;
}

void Utils::withNeighbors(int key)
{

    int numNeighbors;
    int neighbors[NEIGHBORS];
    getNeighbors(key, neighbors, numNeighbors);

    vector<vector<double>> *current_cell = &cells[key]; // current_cell

    for (int i_neighbor = 0; i_neighbor < numNeighbors; i_neighbor++)
    {
        vector<vector<double>> *neighbor = &cells[neighbors[i_neighbor]]; // neighbor of current cell

        for (int j_sphere = 0; j_sphere < neighbor->size(); j_sphere++)
            for (vector<vector<double>>::iterator current_sphere = current_cell->begin(); current_sphere != current_cell->end();)
            {
                if (
                    checkDistanceConstraints(
                        (*current_sphere)[0],     //x1
                        (*neighbor)[j_sphere][0], //x2
                        (*current_sphere)[1],     //y1
                        (*neighbor)[j_sphere][1]) //y2
                )
                    current_sphere = current_cell->erase(current_sphere);
                else
                    current_sphere++;
            }
    }
}

void Utils::getNeighbors(int key, int *neighbors, int &numNeighbors)
{
    numNeighbors = 0;

    if (checkNeighbors(key, LEFT))
        neighbors[numNeighbors++] = key + LEFT;
    if (checkNeighbors(key, RIGHT))
        neighbors[numNeighbors++] = key + RIGHT;
    if (checkNeighbors(key, UP))
        neighbors[numNeighbors++] = key + UP;
    if (checkNeighbors(key, UP_LEFT))
        neighbors[numNeighbors++] = key + UP_LEFT;
    if (checkNeighbors(key, UP_RIGHT))
        neighbors[numNeighbors++] = key + UP_RIGHT;
    if (checkNeighbors(key, DOWN))
        neighbors[numNeighbors++] = key + DOWN;
    if (checkNeighbors(key, DOWN_LEFT))
        neighbors[numNeighbors++] = key + DOWN_LEFT;
    if (checkNeighbors(key, DOWN_RIGHT))
        neighbors[numNeighbors++] = key + DOWN_RIGHT;
}

void Utils::getNeighborsForComputation(int key, int *neighbors, int &numNeighbors)
{
    numNeighbors = 0;

    if (checkNeighbors(key, RIGHT))
        neighbors[numNeighbors++] = key + RIGHT;
    if (checkNeighbors(key, UP))
        neighbors[numNeighbors++] = key + UP;
    if (checkNeighbors(key, UP_LEFT))
        neighbors[numNeighbors++] = key + UP_LEFT;
    if (checkNeighbors(key, UP_RIGHT))
        neighbors[numNeighbors++] = key + UP_RIGHT;

    // cout << key << "  ";
    // for (int i = 0; i < numNeighbors; i++)
    //     cout << neighbors[i] << "  ";
    // cout << endl;
}

bool Utils::checkNeighbors(int key, int newKey)
{

    if (
        (newKey == LEFT && (key % COLUMNS == 0)) ||
        (newKey == RIGHT && ((key + 1) % COLUMNS == 0)) ||
        (newKey == DOWN_LEFT && ((key + newKey < 0) || (key % COLUMNS == 0))) ||
        (newKey == DOWN_RIGHT && ((key + newKey < 0) || ((key + 1) % COLUMNS == 0))) ||
        (newKey == DOWN && (key + newKey < 0)) ||
        (newKey == UP_LEFT && ((key + newKey >= CELLS) || (key % COLUMNS == 0))) ||
        (newKey == UP_RIGHT && ((key + newKey >= CELLS) || ((key + 1) % COLUMNS == 0))) ||
        (newKey == UP && (key + newKey >= CELLS)))
        return false;

    return true;
}

void Utils::exportToFile(const vector<vector<double>> &positions)
{

    ifstream templateFile;
    ofstream outputFile;
    string inputLine;
    string outputLine;
    string normal;

    outputFile.open(outputSphere);

    for (int i = 0; i < positions.size(); i++)
    {
        templateFile.open(templateSphere);
        if (!templateFile)
        {
            cerr << "Error to open file" << endl;
            return;
        }

        getline(templateFile, inputLine); //firstLine ---> solid COMSOL rendering object sph1
        if (i == 0)
            outputFile << inputLine << endl;

        while (getline(templateFile, inputLine))
        {
            if (inputLine.substr(0, 8).compare("endsolid") == 0)
                break;
            // facet normal -0.083596795797348 -0.00131421640980989 -0.996056079864502
            outputFile << inputLine << endl;
            // outer loop
            getline(templateFile, inputLine);
            outputFile << inputLine << endl;
            // vertex -2.50666465362315e-09 3.06977874999577e-25 -1.98422931418918e-08
            getline(templateFile, inputLine);
            outputFile << computeVertex(inputLine, positions[i]) << endl;
            // vertex -1.25581034460254e-09 1.53792414755217e-25 -1.99605345585496e-08
            getline(templateFile, inputLine);
            outputFile << computeVertex(inputLine, positions[i]) << endl;
            // vertex -1.25333232681157e-09 -7.88529866402321e-11 -1.99605345585496e-08
            getline(templateFile, inputLine);
            outputFile << computeVertex(inputLine, positions[i]) << endl;
            // endloop
            getline(templateFile, inputLine);
            outputFile << inputLine << endl;
            // endfacet
            getline(templateFile, inputLine);
            outputFile << inputLine << endl;
        }
        templateFile.close();
    }
    //endsolid COMSOL rendering object sph1
    outputFile << inputLine << endl;
    //close

    outputFile.close();
}

bool Utils::canStartSimulation()
{
    return !(ROW_DIVIDER < Utils::maxDistanceBetweenParticles || COL_DIVIDER < Utils::maxDistanceBetweenParticles);
}

void Utils::exportResultToFile(vector<double> &Qsca, vector<double> &Qabs, vector<double> &Qext_print_single, vector<double> &Qext_print_double, vector<double> &Qext_print_all, const vector<vector<complex<double>>> &S1, const vector<vector<complex<double>>> &S2, int nang, int threads, double avg_time_elapsed)
{
    ofstream outputFile;
    string outputLine;
    stringstream tmp;

    string file = resultFile + "_" + to_string(threads) + "_" + to_string(positions.size()) + "_" + to_string(Utils::end) + "_" + to_string(avg_time_elapsed) + ".txt";

    outputFile.open(file);

    tmp << setprecision(10);
    outputFile << setprecision(10);

    for (int i = 0; i < Qsca.size(); i++)
    {
        tmp << Qsca[i] << " ";
        tmp << Qabs[i] << " ";
        tmp << Qext_print_single[i] << " " << Qext_print_double[i] << " " << Qext_print_all[i] << " ";
        for (int j = 0; j < nang; j++)
            tmp << S1[i][j] << " ";
        for (int j = 0; j < nang; j++)
            tmp << S2[i][j] << " ";
        tmp << endl;
    }

    outputLine = tmp.str();
    outputFile << outputLine << endl;
    outputFile.close();
}

void Utils::check_single_double(vector<vector<double> *> &singleParticles, map<vector<double> *, vector<vector<double> *>> &doubleParticles, vector<vector<double> *> &keys, bool pol)
{

    map<int, vector<vector<double>>> &cells = Utils::cells;

    for (int key = 0; key < CELLS; key++)
    {
        //int key = it->first;
        int numNeighbors;
        int neighbors[NEIGHBORS];
        Utils::getNeighborsForComputation(key, neighbors, numNeighbors);

        for (int i = 0; i < cells[key].size(); i++)
        {
            bool single = true;

            //particelle nella stessa cella
            for (int j = i + 1; j < cells[key].size(); j++)
                if (Utils::checkDistanceBetweenParticles(cells[key][i][0], cells[key][i][1], cells[key][j][0],
                                                         cells[key][j][1], pol))
                {
                    doubleParticles[&cells[key][i]].push_back(&cells[key][j]);
                    single = false;
                }
            //particelle nelle celle vicine
            for (int current = 0; current < numNeighbors; current++)       //per ogni vicino
                for (int j = 0; j < cells[neighbors[current]].size(); j++) //per ogni particella di ogni vicino
                    if (Utils::checkDistanceBetweenParticles(cells[key][i][0], cells[key][i][1],
                                                             cells[neighbors[current]][j][0],
                                                             cells[neighbors[current]][j][1], pol))
                    {
                        doubleParticles[&cells[key][i]].push_back(&cells[neighbors[current]][j]);
                        single = false;
                    }
            if (single)
                singleParticles.push_back(&cells[key][i]);
        } //particle on current cell

    } //cells

    for (map<vector<double> *, vector<vector<double> *>>::iterator it = doubleParticles.begin(); it != doubleParticles.end(); it++)
        keys.push_back(it->first);
}

string Utils::computeVertex(const string &inputLine, const vector<double> &position)
{
    // if we want to use scientific notation
    // <<scientific
    //precision for all digits
    cout << setprecision(15);
    ostringstream doubleNumber;
    //precision for all digits
    doubleNumber.precision(15);

    string outputLine;
    stringstream check(inputLine);
    string intermediate;

    //vertex
    getline(check, intermediate, ' ');
    outputLine += intermediate + ' ';

    //x
    getline(check, intermediate, ' ');
    doubleNumber << (stod(intermediate) + position[0]);
    outputLine += doubleNumber.str() + ' ';
    doubleNumber.str("");
    doubleNumber.clear();

    //y
    getline(check, intermediate, ' ');
    doubleNumber << (stod(intermediate) + position[1]);
    outputLine += doubleNumber.str() + ' ';
    doubleNumber.str("");
    doubleNumber.clear();
    //z
    getline(check, intermediate, ' ');
    doubleNumber << (stod(intermediate) + position[2]);
    outputLine += doubleNumber.str() + '\r';
    doubleNumber.str("");
    doubleNumber.clear();

    return outputLine;
}
