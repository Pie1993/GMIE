#ifndef DEM_H
#define DEM_H

#include <iostream>
using namespace std;
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <string>
#include <Utils.h>
#include <Simulation.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// #define VERBOSE
 #define LINEMODE

class Dem
{
public:
  Dem();
  ~Dem();
  bool getPatternChosen()const;

protected:
  char *readFile(string);

  bool loadOBJ(const char *path);


  char infoLog[512];

  int vertexShader_sphere;
  int fragmentShader_sphere;

  char *vertexShaderSource;
  char *fragmentShaderSource;

  

  float l = 1;
  float half_l = l / 2;

  float *data;
  float *verticeNormals;

  float default_R_color = 0.0f;
  float default_G_color = 0.0f;
  float default_B_color = 0.0f;

  vector<glm::vec3> verticesSphere;
  vector<glm::vec2> uvsSphere;
  vector<glm::vec3> normalsSphere; // Won't be used at the moment.
  vector<glm::vec3> totalDataSphere;



 
  //static members

  static bool patternChosen;	
  static bool updatePattern;
  static float distance;
  static float rotationRight;
  static float rotationUp;
  static bool movementActive;
  static float scaleX;
  static float scaleY;
  static float scaleZ;
  static float scaleFactor;
  static float translationPlanX;
  static float translationPlanY;
  static float translationPlanZ;
  static float translationFactor;
  static bool zoomActive;
  static double oldMouseX;
  static double oldMouseY;
  static bool activeRotation;
  static int deltaX;
  static int deltaY;
  static float rotationX;
  static float rotationY;
  static float rotationSensibility;
  static bool moveBackward;
  static bool moveForward;
  static bool moveRight;
  static bool moveLeft;
  static float deltaMove;
  static float deltaAngle;
  static float angle;
  static float yaw;
  static float pitch;
  static float deltaTime;
  static float lastFrame;
  static float speed;
  static bool firstMouse;
  static float fov;
  static float increment;
  static float rotIncrement;

  static bool simulation_pause;

  static unsigned int SCR_WIDTH;
  static unsigned int SCR_HEIGHT;
  static unsigned int SIZE;

  static string shadersFolder;
  static string vertexFileNameSphere;
  static string fragmentFileNameSphere;

};
#endif
