#include "OpenGlDem.h"

glm::vec3 OpenGlDem::cameraPos = glm::vec3(distance, 0, 0);
glm::vec3 OpenGlDem::cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 OpenGlDem::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 OpenGlDem::cameraRight = glm::normalize(glm::cross(cameraUp, cameraFront));
double OpenGlDem::factor_scale = 0.0015; //0.0039 o //0.0031 fattore di scala del modello per coincidere con le dimensioni reali

OpenGlDem::OpenGlDem()
{
    status = false;
    status = openGlDemInit();

    if (status)
    {
        loadModel();
        setOpenGlBufferData();
    }
}

OpenGlDem::~OpenGlDem()
{
    glDeleteVertexArrays(1, &VAO3);
    glDeleteBuffers(1, &VBO3);
    glfwTerminate();
}

bool OpenGlDem::getStatus() const
{
    return status;
}

void OpenGlDem::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void OpenGlDem::processInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{

    if (action == GLFW_REPEAT || action == GLFW_PRESS)
        pressAction(window, key);
    else if (action == GLFW_RELEASE)
        releaseAction(window, key);
}

void OpenGlDem::releaseAction(GLFWwindow *window, int key)
{
    switch (key)
    {
    case GLFW_KEY_LEFT_CONTROL:
        movementActive = false;
        moveForward = false;
        moveBackward = false;
        deltaMove = 0;
        break;

    case GLFW_KEY_UP:
        moveForward = false;
        deltaMove = 0;
        break;

    case GLFW_KEY_DOWN:
        deltaMove = 0;
        moveBackward = false;
        break;

    case GLFW_KEY_ENTER:
        updatePattern = true;
        break;
    }
}

void OpenGlDem::pressAction(GLFWwindow *window, int key)
{

    switch (key)
    {

    case GLFW_KEY_S:
        patternChosen=true;
        glfwSetWindowShouldClose(window, true);
        break;

    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;

    case GLFW_KEY_UP:
        fov -= increment;
        break;

    case GLFW_KEY_DOWN:
        fov += increment;
        break;
    }
}

bool OpenGlDem::createVertexShader(int &vertexShader, int &success)
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
             << infoLog << endl;
        return false;
    }
    return true;
}

bool OpenGlDem::createFragmentShader(int &fragmentShader, int &success)
{
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<< infoLog << endl;
        return false;
    }
    return true;
}

void OpenGlDem::transformVertices(int &shaderProgram)
{
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(-distance, -Utils::end - Utils::end - Utils::end / 2, Utils::end + Utils::end + Utils::end / 2));
    // model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    model = glm::rotate(model, yaw, glm::vec3(0, 1, 0));
    model = glm::rotate(model, pitch, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

    view = glm::lookAt(cameraPos, cameraFront, glm::vec3(0, 1, 0));
    projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    model = glm::translate(model, glm::vec3((double)newCoordinates[0],(double)newCoordinates[1],0.0));
    model = glm::scale(model, glm::vec3(Utils::end * factor_scale, Utils::end * factor_scale, Utils::end * factor_scale));

    unsigned int modelLoc_uniform = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc_uniform = glGetUniformLocation(shaderProgram, "view");
    unsigned int projection_uniform = glGetUniformLocation(shaderProgram, "projection");
    unsigned int viewPos_uniform = glGetUniformLocation(shaderProgram, "viewPos");

    glUniformMatrix4fv(modelLoc_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc_uniform, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(viewPos_uniform, cameraPos.x, cameraPos.y, cameraPos.z);
}

bool OpenGlDem::openGlDemInit()
{
    int success = false;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mie scattering", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, this->framebuffer_size_callback);
    glfwSetWindowPos(this->window, 400, -500);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return false;
    }

    //add event call_back function
    glfwSetKeyCallback(this->window, this->processInput);

    glEnable(GL_DEPTH_TEST);


    //Sphere----------------------------------------------------------------------------

    vertexShaderSource = readFile(vertexFileNameSphere);
    fragmentShaderSource = readFile(fragmentFileNameSphere);

    // // build and compile our shader program
    // // ------------------------------------
    // // vertex shader
    createVertexShader(this->vertexShader_sphere, success);
    // // fragment shader
    createFragmentShader(this->fragmentShader_sphere, success);

    // // link shaders
    shaderProgram_sphere = glCreateProgram();
    glAttachShader(shaderProgram_sphere, vertexShader_sphere);
    glAttachShader(shaderProgram_sphere, fragmentShader_sphere);
    glLinkProgram(shaderProgram_sphere);
    // // check for linking errors
    glGetProgramiv(shaderProgram_sphere, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram_sphere, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
             << infoLog << endl;
        return false;
    }
    glDeleteShader(vertexShader_sphere);
    glDeleteShader(fragmentShader_sphere);



    return true;
}

void OpenGlDem::setOpenGlBufferData()
{
  
#ifdef LINEMODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    //sphere----------------------------------------------------
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);
    // glGenBuffers(1, &EBO3);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glBindVertexArray(VAO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, totalDataSphere.size() * sizeof(glm::vec3), &totalDataSphere[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void *)(0 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void *)(1 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlDem::loadModel()
{
    loadOBJ("../Sphere.obj");

    for (int i = 0; i < verticesSphere.size(); i++)
    {
        totalDataSphere.push_back(verticesSphere[i]);
        totalDataSphere.push_back(normalsSphere[i]);
    }
}

void OpenGlDem::openGlLoopUpdate()
{

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

vector<vector<double>> positions;
    while (!glfwWindowShouldClose(window))
    {
        if (updatePattern)
        {
            positions.clear();
            positions = Utils::generatePattern();
            updatePattern = false;
        }
        // input
        // -----
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // render
        // ------
        glClearColor(default_R_color, default_G_color, default_B_color, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //cout << "loop" << endl;
        //-----------------Sphere-------------------------------
        for (int i = 0; i < positions.size(); i++)
        {
            glUseProgram(shaderProgram_sphere);
            glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need
            newCoordinates =  positions[i];
            transformVertices(shaderProgram_sphere);
            glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}