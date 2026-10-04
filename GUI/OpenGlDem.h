#ifndef OPENGLDEM_H
#define OPENGLDEM_H
#include "Dem.h"

class OpenGlDem: public Dem
{

public:
OpenGlDem();
~OpenGlDem();
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window,int key, int scancode, int action, int mods);
static void pressAction(GLFWwindow *window,int key);
static void releaseAction(GLFWwindow *window,int key);
bool createVertexShader(int&vertexShader,int&success);
bool createFragmentShader(int&fragmentShader,int&success);
void transformVertices(int & shaderProgram);
void loadModel();
bool openGlDemInit();
void setOpenGlBufferData();
void initData();
void openGlLoopUpdate();
bool getStatus() const;

protected:
static double factor_scale;
vector<double> newCoordinates;

static glm::vec3 cameraPos; 
static glm::vec3 cameraRight;
static glm::vec3 cameraDirection;
static glm::vec3 cameraUp;
static glm::vec3 cameraFront;


bool status;
GLFWwindow* window;
int shaderProgram_sphere;

unsigned int VBO3;
unsigned int VAO3;
};
#endif