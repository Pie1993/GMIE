#include "Dem.h"
unsigned int Dem::SCR_WIDTH = 1800;
unsigned int Dem::SCR_HEIGHT = 1600;
unsigned int Dem::SIZE = 1024;
string Dem::shadersFolder = "../Shaders/";
string Dem::vertexFileNameSphere = "vertexShaderSphere.vs";
string Dem::fragmentFileNameSphere = "fragmentShaderSphere.vs";

bool Dem::updatePattern = true;
bool Dem::patternChosen = false;
float Dem::rotationUp = 0;
float Dem::scaleX = 1;
float Dem::scaleY = 1;
float Dem::scaleZ = 1;
float Dem::scaleFactor = 0.001f;
float Dem::yaw = 90.0f;
float Dem::pitch = 0.0f;
float Dem::deltaTime = 0.0f; // time between current frame and last frame
float Dem::lastFrame = 0.0f;
float Dem::speed = 50.0f;
float Dem::increment = 0.0001f;
float Dem::rotIncrement = 0.5f;

float Dem::translationPlanX = 0.0f;
float Dem::translationPlanY = 0.0f;
float Dem::translationPlanZ = 0.0f;
bool Dem::simulation_pause = true;
bool Dem::zoomActive = false;
float Dem::translationFactor = 0.005f;
double Dem::oldMouseX = 0;
double Dem::oldMouseY = 0;
int Dem::deltaX = 0;
int Dem::deltaY = 0;
float Dem::rotationX = 0;
float Dem::rotationY = 0;
float Dem::rotationRight = 0;
float Dem::rotationSensibility = 1;
bool Dem::movementActive = false;
bool Dem::activeRotation = false;
bool Dem::moveForward = false;
bool Dem::moveBackward = false;
bool Dem::moveRight = false;
bool Dem::moveLeft = false;
float Dem::deltaMove = 0.0f;
float Dem::deltaAngle = 0.0f;
float Dem::angle = 0.0f;
bool Dem::firstMouse = true;
float Dem::distance = 2;
float Dem::fov = 0.025f;

Dem::Dem()
{

}
Dem::~Dem()
{

    delete vertexShaderSource;
    delete fragmentShaderSource;

    vertexShaderSource = 0;
    fragmentShaderSource = 0;
}



char *Dem::readFile(string fileName)
{
    cout << fileName << endl;
    string shaderSource;
    string line;
    fstream file;
    file.open(shadersFolder + fileName, fstream::in);

    if (!file)
    {
        cout << "Error open file" << endl;
        exit(1);
    }

    while (getline(file, line))
    {
        shaderSource += line + "\n";
    }
    shaderSource += "\0";
    file.close();

    char *p = new char[shaderSource.length() + 1];
    strcpy(p, shaderSource.c_str());
    return p;
}

bool Dem::loadOBJ(const char *path)
{

    printf("Loading OBJ file %s...\n", path);

    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        getchar();
        return false;
    }

    while (1)
    {

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9)
            {
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                fclose(file);
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        // Put the attributes in buffers
        verticesSphere.push_back(vertex);
        uvsSphere.push_back(uv);
        normalsSphere.push_back(normal);
    }
    fclose(file);
    return true;
}

bool Dem::getPatternChosen()const
{
    return patternChosen;
}	