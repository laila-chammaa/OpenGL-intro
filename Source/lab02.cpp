//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>


#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <fstream>
#include <string>
#include <sstream>

const char* getVertexShaderSource()
{
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        ""
        ""
        "uniform vec3 aColor = vec3(1.0f, 1.0f, 1.0f);"
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
}


const char* getFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "in vec3 vertexColor;"
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
        "}";
}


int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

#pragma region VAOs

// laila's colors!
int createVertexArrayObject2()
{
    // A vertex is a point on a polygon, it contains positions and other data (eg: colors)
    glm::vec3 vertexArray[] = {  // position,                            color
        glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(0.3f, 0.0f, 0.6f), //left - red
        glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3(0.3f, 0.0f, 0.6f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.3f, 0.0f, 0.6f),

        glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(0.3f, 0.0f, 0.6f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.3f, 0.0f, 0.6f),
        glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(0.3f, 0.0f, 0.6f),

        glm::vec3(0.5f, 0.5f,-0.5f), glm::vec3(0.0f, 0.0f,  0.5f), // far - blue
        glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(0.0f, 0.0f, 0.5f),
        glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(0.0f, 0.0f, 0.5f),

        glm::vec3(0.5f, 0.5f,-0.5f), glm::vec3(0.0f, 0.0f,  0.5f),
        glm::vec3(0.5f,-0.5f,-0.5f), glm::vec3(0.0f, 0.0f,  0.5f),
        glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(0.0f, 0.0f, 0.5f),

        glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(0.7f,  0.0f, 1.0f), // bottom - turquoise
        glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(0.7f, 0.0f, 1.0f),
        glm::vec3(0.5f,-0.5f,-0.5f), glm::vec3(0.7f,  0.0f, 1.0f),

        glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(0.7f,  0.0f, 1.0f),
        glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3(0.7f, 0.0f, 1.0f),
        glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(0.7f, 0.0f, 1.0f),

        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.4f), // near - pink
        glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.4f),
        glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.4f),

        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.4f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.4f),
        glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.4f),

        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 1.0f), // right - purple
        glm::vec3(0.5f,-0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f),

        glm::vec3(0.5f,-0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 1.0f),

        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), // top - yellow
        glm::vec3(0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 1.0f, 0.0f),

        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f)
    };

    // Create a vertex array
    GLuint vertexArrayObject2;
    glGenVertexArrays(1, &vertexArrayObject2);
    glBindVertexArray(vertexArrayObject2);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(glm::vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(glm::vec3),
        (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject2;
}
#pragma endregion

int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    glfwWindowHint(GLFW_SAMPLES, 4); // creates more buffers for model smoothing
    // Create Window and rendering context using GLFW, resolution is 1024x768
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Comp371 - Lab 02", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();
    glUseProgram(shaderProgram);
    //feild of vew variable
    float feild_of_vew = 70.0f;
    float temp_feild_of_vew = 70.0f;
    // Set projection matrix for shader, this won't change
    glm::mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
        1024.0f / 768.0f,  // aspect ratio
        0.01f, 100.0f);   // near and far (near > 0)

    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    //params
    // Camera parameters for view transform
    glm::vec3 cameraPosition = glm::vec3(0.6f, 1.0f, 10.0f);
    glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Set initial view matrix
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    // Define and upload geometry to the GPU here ...
    int vao2 = createVertexArrayObject2();

    float rotationSpeed = 180.0f;  // 180 degrees per second
    float lastFrameTime = glfwGetTime();

#pragma region Variables of models
    bool isPressedA = false;
    bool isPressedD = false;
    bool isPressedU = false;
    bool isPressedJ = false;
    float rotationAngle = 10.0f;

    bool CModelIsOn = false;
    float CAnglex = 0;
    float CAngley = 0;
    float CMovex = 0;
    float CMovey = 0;
    float CScale = 1;

    bool HModelIsOn = false;
    float HAnglex = 0;
    float HAngley = 0;
    float HMovex = 0;
    float HMovey = 0;
    float HScale = 1;

    bool  A1ModelIsOn = false;
    float A1Anglex = 0;
    float A1Angley = 0;
    float A1Movex = 0;
    float A1Movey = 0;
    float A1Scale = 1;

    bool  M1ModelIsOn = false;
    float M1Anglex = 0;
    float M1Angley = 0;
    float M1Movex = 0;
    float M1Movey = 0;
    float M1Scale = 1;

    bool  M2ModelIsOn = false;
    float M2Anglex = 0;
    float M2Angley = 0;
    float M2Movex = 0;
    float M2Movey = 0;
    float M2Scale = 1;

    bool  A2ModelIsOn = false;
    float A2Anglex = 0;
    float A2Angley = 0;
    float A2Movex = 0;
    float A2Movey = 0;
    float A2Scale = 1;

    //for rotating the world
    float worldAnglex = 0;
    float worldAngley = 0;

    float modelMovementSpeed = 10.0f;

#pragma endregion

    // backface culling
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    // Enable Depth test for proper rendering
    glEnable(GL_DEPTH_TEST);
    // Enable Multisample to smoothen edges on models and grid lines
    glEnable(GL_MULTISAMPLE);

    float cameraSpeed = 20.0f;
    float cameraFastSpeed = 2 * cameraSpeed;

    //enum value for changing from triangles to points and lines
    GLenum draw = GL_TRIANGLES;

    //cam x angle, used for mouse movements
    float camx = 1.57;
    float tempcamx = 1.57;
    //cam y angle
    float camy = 0;
    float tempcamy = 0;

    //mouse position
    double tempxpos, tempypos;
    GLuint aColorLocation = glGetUniformLocation(shaderProgram, "aColor");

    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw grid
        glUniform3f(aColorLocation, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 200; i++) {

            glBegin(GL_LINES);
            if (i < 100) { glVertex3f(-50, -0.1, 50 - i); glVertex3f(50, -0.1, 50 - i); }
            else { glVertex3f(150 - i, -0.1, -50); glVertex3f(150 - i, -0.1, 50); }

            glEnd();
            //glPopMatrix();

        }

        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

#pragma region World
        glm::mat4 worldContructionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 worldRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(worldAnglex), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(worldAngley), glm::vec3(0.0f, 1.0f, 0.0f));
        worldContructionMatrix = worldContructionMatrix * worldRotationMatrix;
        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldContructionMatrix[0][0]);
#pragma endregion

        //reset transformation
        //this will make the grid move independantly from the models
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))[0][0]);

#pragma region C
        glBindVertexArray(vao2);
        glUniform3f(aColorLocation, 0.9f, 0.5f, 0.7f); //color of C
        glm::mat4 CRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(CAnglex), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(CAngley), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 CGroupMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f + CMovex, 0.2f + CMovey, -20.0f)) * CRotationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(CScale, CScale, CScale));
        // C H A M M A
        // start of C
        glm::mat4 CPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        CPartMatrix = worldRotationMatrix * CGroupMatrix * CPartMatrix;
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        CPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        CPartMatrix = worldRotationMatrix * CGroupMatrix * CPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        CPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75f, 0.75f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        CPartMatrix = worldRotationMatrix * CGroupMatrix * CPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);
        // end of C
#pragma endregion
#pragma region H
        glUniform3f(aColorLocation, 0.2f, 0.0f, 0.1f); // H color

        // start of H
        glm::mat4 HRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(HAnglex), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(HAngley), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 HGroupMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f + HMovex, 0.2f + HMovey, -20.0f)) * HRotationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(HScale, HScale, HScale));

        glm::mat4 HPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        HPartMatrix = worldRotationMatrix * HGroupMatrix * HPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &HPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        HPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        HPartMatrix = worldRotationMatrix * HGroupMatrix * HPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &HPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);


        HPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.15f, 0.75f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        HPartMatrix = worldRotationMatrix * HGroupMatrix * HPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &HPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);


        HPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        HPartMatrix = worldRotationMatrix * HGroupMatrix * HPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &HPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        HPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        HPartMatrix = worldRotationMatrix * HGroupMatrix * HPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &HPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        // end of H
#pragma endregion
#pragma region A1
        glUniform3f(aColorLocation, 0.1f, 0.0f, 0.4f); // A1 color
        // start of A
        glm::mat4 ARotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(A1Anglex), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(A1Angley), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 AGroupMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f + A1Movex, 0.2f + A1Movey, -20.0f)) * ARotationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(A1Scale, A1Scale, A1Scale));

        glm::mat4 APartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.51f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        APartMatrix = worldRotationMatrix * AGroupMatrix * APartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &APartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        APartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.51f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        APartMatrix = worldRotationMatrix * AGroupMatrix * APartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &APartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);



        APartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        APartMatrix = worldRotationMatrix * AGroupMatrix * APartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &APartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        APartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.76f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        APartMatrix = worldRotationMatrix * AGroupMatrix * APartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &APartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);


        APartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.51f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        APartMatrix = worldRotationMatrix * AGroupMatrix * APartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &APartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        APartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.51f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        APartMatrix = worldRotationMatrix * AGroupMatrix * APartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &APartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        // end of A
#pragma endregion
#pragma region M1
        glUniform3f(aColorLocation, 0.7f, 0.5f, 0.8f); // M1 color
        // start of M
        glm::mat4 MRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(M1Anglex), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(M1Angley), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 MGroupMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f + M1Movex, 0.2f + M1Movey, -20.0f)) * MRotationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(M1Scale, M1Scale, M1Scale));


        glm::mat4 MPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        MPartMatrix = worldRotationMatrix * MGroupMatrix * MPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &MPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        MPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        MPartMatrix = worldRotationMatrix * MGroupMatrix * MPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &MPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);


        MPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        MPartMatrix = worldRotationMatrix * MGroupMatrix * MPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &MPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        MPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        MPartMatrix = worldRotationMatrix * MGroupMatrix * MPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &MPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);


        MPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        MPartMatrix = worldRotationMatrix * MGroupMatrix * MPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &MPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        MPartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        MPartMatrix = worldRotationMatrix * MGroupMatrix * MPartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &MPartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);
        // END OF M
#pragma endregion
#pragma region M2
        glUniform3f(aColorLocation, 0.2f, 0.2f, 0.8f); // M2 color
         // start of M
        glm::mat4 M2RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(M2Anglex), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(M2Angley), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 M2GroupMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.56f + M2Movex, 0.2f + M2Movey, -20.0f)) * M2RotationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(M2Scale, M2Scale, M2Scale));

        glm::mat4 M2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        M2PartMatrix = worldRotationMatrix * M2GroupMatrix * M2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &M2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        M2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        M2PartMatrix = worldRotationMatrix * M2GroupMatrix * M2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &M2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);




        M2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        M2PartMatrix = worldRotationMatrix * M2GroupMatrix * M2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &M2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        M2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        M2PartMatrix = worldRotationMatrix * M2GroupMatrix * M2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &M2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);




        M2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        M2PartMatrix = worldRotationMatrix * M2GroupMatrix * M2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &M2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        M2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        M2PartMatrix = worldRotationMatrix * M2GroupMatrix * M2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &M2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        //END OF M
#pragma endregion
#pragma region A2
        glUniform3f(aColorLocation, 0.8f, 0.4f, 0.8f); // A2 color
        // start of A
        glm::mat4 A2RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(A2Anglex), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(A2Angley), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 A2GroupMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f + A2Movex, 0.2f + A2Movey, -20.0f)) * A2RotationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(A2Scale, A2Scale, A2Scale));

        glm::mat4 A2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.49f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        A2PartMatrix = worldRotationMatrix * A2GroupMatrix * A2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &A2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        A2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.49f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        A2PartMatrix = worldRotationMatrix * A2GroupMatrix * A2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &A2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        A2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.76f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        A2PartMatrix = worldRotationMatrix * A2GroupMatrix * A2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &A2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        A2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        A2PartMatrix = worldRotationMatrix * A2GroupMatrix * A2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &A2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);


        A2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.51f, 1.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        A2PartMatrix = worldRotationMatrix * A2GroupMatrix * A2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &A2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        A2PartMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.51f, 0.2f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.25f));
        A2PartMatrix = worldRotationMatrix * A2GroupMatrix * A2PartMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &A2PartMatrix[0][0]);
        glDrawArrays(draw, 0, 36);

        // end of A

#pragma endregion

#pragma region gridAxis
        glUniform3f(aColorLocation, 1.0f, 0.0f, 0.0f); // grid red
        //x-axis - red
        glm::mat4 axisMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.25f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.12f, 0.12f));
        axisMatrix = worldRotationMatrix * axisMatrix;
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
        glDrawArrays(draw, 0, 36);
        //y-axis
        glUniform3f(aColorLocation, 0.0f, 1.0f, 0.0f); // grid green
        axisMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.25f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.12f, 3.0f, 0.12f));
        axisMatrix = worldRotationMatrix * axisMatrix;
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
        glDrawArrays(draw, 0, 36);
        //z-axis
        glUniform3f(aColorLocation, 1.0f, 1.0f, 0.0f); // grid yellow
        axisMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.25f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.12f, 0.12f, 3.0f));
        axisMatrix = worldRotationMatrix * axisMatrix;
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
        glDrawArrays(draw, 0, 36);
#pragma endregion


        //dot to fix the grid problem of attaching to the last drawn point
        glm::mat4 dot = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(worldAnglex), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(worldAngley), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &dot[0][0]);
        glDrawArrays(GL_POINTS, 0, 36);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

        bool fastCam = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

        //Rotate model that is selected along the Y-axis 5 degrees per key press
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !isPressedA) // rotate right 5 degrees
        {
            isPressedA = true;

            if (CModelIsOn)
                CAnglex += rotationAngle;
            else if (HModelIsOn)
                HAnglex += rotationAngle;
            else if (A1ModelIsOn)
                A1Anglex += rotationAngle;
            else if (M1ModelIsOn)
                M1Anglex += rotationAngle;
            else if (M2ModelIsOn)
                M2Anglex += rotationAngle;
            else if (A2ModelIsOn)
                A2Anglex += rotationAngle;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !isPressedD)
        {
            isPressedD = true;

            if (CModelIsOn)
                CAnglex -= rotationAngle;
            else if (HModelIsOn)
                HAnglex -= rotationAngle;
            else if (A1ModelIsOn)
                A1Anglex -= rotationAngle;
            else if (M1ModelIsOn)
                M1Anglex -= rotationAngle;
            else if (M2ModelIsOn)
                M2Anglex -= rotationAngle;
            else if (A2ModelIsOn)
                A2Anglex -= rotationAngle;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && isPressedA)
        {
            isPressedA = false;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE && isPressedD)
        {
            isPressedD = false;
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) // rotate left by 5 degrees (continous rotation!)
        {

            if (CModelIsOn)
                CAnglex += CAnglex + rotationSpeed * dt;
            else if (HModelIsOn)
                HAnglex += CAnglex + rotationSpeed * dt;
            else if (A1ModelIsOn)
                A1Anglex += A1Anglex + rotationSpeed * dt;
            else if (M1ModelIsOn)
                M1Anglex += M1Anglex + rotationSpeed * dt;
            else if (M2ModelIsOn)
                M2Anglex += M2Anglex + rotationSpeed * dt;
            else if (A2ModelIsOn)
                A2Anglex += A2Anglex + rotationSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) // rotate right by 5 degrees (continous rotation!)
        {
            if (CModelIsOn)
                CAnglex = CAnglex - rotationSpeed * dt;
            else if (HModelIsOn)
                HAnglex = HAnglex - rotationSpeed * dt;
            else if (A1ModelIsOn)
                A1Anglex = A1Anglex - rotationSpeed * dt;
            else if (M1ModelIsOn)
                M1Anglex = M1Anglex - rotationSpeed * dt;
            else if (M2ModelIsOn)
                M2Anglex = M2Anglex - rotationSpeed * dt;
            else if (A2ModelIsOn)
                A2Anglex = A2Anglex - rotationSpeed * dt;
        }

        //Rotate model that is selected along the X-axis 5 degrees per key press (continuously)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            if (CModelIsOn)
                CAngley = CAngley + rotationSpeed * dt;
            else if (HModelIsOn)
                HAngley = HAngley + rotationSpeed * dt;
            else if (A1ModelIsOn)
                A1Angley = A1Angley + rotationSpeed * dt;
            else if (M1ModelIsOn)
                M1Angley = M1Angley + rotationSpeed * dt;
            else if (M2ModelIsOn)
                M2Angley = M2Angley + rotationSpeed * dt;
            else if (A2ModelIsOn)
                A2Angley = A2Angley + rotationSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            if (CModelIsOn)
                CAngley = CAngley - rotationSpeed * dt;
            else if (HModelIsOn)
                HAngley = HAngley - rotationSpeed * dt;
            else if (A1ModelIsOn)
                A1Angley = A1Angley - rotationSpeed * dt;
            else if (M1ModelIsOn)
                M1Angley = M1Angley - rotationSpeed * dt;
            else if (M2ModelIsOn)
                M2Angley = M2Angley - rotationSpeed * dt;
            else if (A2ModelIsOn)
                A2Angley = A2Angley - rotationSpeed * dt;
        }

        //move selected model with shift + a,d,w,s
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)   // move model left 
        {
            if (CModelIsOn)
                CMovex = CMovex - modelMovementSpeed * dt;
            else if (HModelIsOn)
                HMovex = HMovex - modelMovementSpeed * dt;
            else if (A1ModelIsOn)
                A1Movex = A1Movex - modelMovementSpeed * dt;
            else if (M1ModelIsOn)
                M1Movex = M1Movex - modelMovementSpeed * dt;
            else if (M2ModelIsOn)
                M2Movex = M2Movex - modelMovementSpeed * dt;
            else if (A2ModelIsOn)
                A2Movex = A2Movex - modelMovementSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // move model up
        {
            if (CModelIsOn)
                CMovey = CMovey + modelMovementSpeed * dt;
            else if (HModelIsOn)
                HMovey = HMovey + modelMovementSpeed * dt;
            else if (A1ModelIsOn)
                A1Movey = A1Movey + modelMovementSpeed * dt;
            else if (M1ModelIsOn)
                M1Movey = M1Movey + modelMovementSpeed * dt;
            else if (M2ModelIsOn)
                M2Movey = M2Movey + modelMovementSpeed * dt;
            else if (A2ModelIsOn)
                A2Movey = A2Movey + modelMovementSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)   // move model down
        {
            if (CModelIsOn)
                CMovey = CMovey - modelMovementSpeed * dt;
            else if (HModelIsOn)
                HMovey = HMovey - modelMovementSpeed * dt;
            else if (A1ModelIsOn)
                A1Movey = A1Movey - modelMovementSpeed * dt;
            else if (M1ModelIsOn)
                M1Movey = M1Movey - modelMovementSpeed * dt;
            else if (M2ModelIsOn)
                M2Movey = M2Movey - modelMovementSpeed * dt;
            else if (A2ModelIsOn)
                A2Movey = A2Movey - modelMovementSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)     // move model right
        {
            if (CModelIsOn)
                CMovex = CMovex + modelMovementSpeed * dt;
            else if (HModelIsOn)
                HMovex = HMovex + modelMovementSpeed * dt;
            else if (A1ModelIsOn)
                A1Movex = A1Movex + modelMovementSpeed * dt;
            else if (M1ModelIsOn)
                M1Movex = M1Movex + modelMovementSpeed * dt;
            else if (M2ModelIsOn)
                M2Movex = M2Movex + modelMovementSpeed * dt;
            else if (A2ModelIsOn)
                A2Movex = A2Movex + modelMovementSpeed * dt;
        }

        //scale model up and down with shift + j,u. one change per key press
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !isPressedU) {

            isPressedU = true;

            if (CModelIsOn)
                CScale += 0.05;
            else if (HModelIsOn)
                HScale += 0.05;
            else if (A1ModelIsOn)
                A1Scale += 0.05;
            else if (M1ModelIsOn)
                M1Scale += 0.05;
            else if (M2ModelIsOn)
                M2Scale += 0.05;
            else if (A2ModelIsOn)
                A2Scale += 0.05;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !isPressedJ) {

            isPressedJ = true;

            if (CModelIsOn)
                CScale -= 0.05;
            else if (HModelIsOn)
                HScale -= 0.05;
            else if (A1ModelIsOn)
                A1Scale -= 0.05;
            else if (M1ModelIsOn)
                M1Scale -= 0.05;
            else if (M2ModelIsOn)
                M2Scale -= 0.05;
            else if (A2ModelIsOn)
                A2Scale -= 0.05;
        }

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE && isPressedU)
        {
            isPressedU = false;
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE && isPressedJ)
        {
            isPressedJ = false;
        }

        // camera movements

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // move camera to the left
        {
            cameraPosition.x -= currentCameraSpeed * dt * sinf(camx);
            cameraPosition.z -= currentCameraSpeed * dt * cosf(camx);

        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.x += currentCameraSpeed * dt * sinf(camx);
            cameraPosition.z += currentCameraSpeed * dt * cosf(camx);
        }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // move camera backward
        {
            cameraPosition.z += currentCameraSpeed * dt * sinf(camx);
            cameraPosition.x -= currentCameraSpeed * dt * cosf(camx);
        }

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // move camera forward
        {
            cameraPosition.z -= currentCameraSpeed * dt * sinf(camx);
            cameraPosition.x += currentCameraSpeed * dt * cosf(camx);
        }

        //moving camera up ad down with [ , ]
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) // move camera up
        {
            cameraPosition.y += currentCameraSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) // move camera down
        {
            cameraPosition.y -= currentCameraSpeed * dt;
        }

        // switching between models with 1-5, camera focuses on model

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // flag C model
        {
            //reseting camera and world angle
            //cam x angle
            camx = 1.57;
            tempcamx = 1.57;
            //cam y angle
            camy = 0;
            tempcamy = 0;

            worldAnglex = 0;
            worldAnglex = 0;
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            CModelIsOn = true;
            HModelIsOn = false;
            A1ModelIsOn = false;
            M1ModelIsOn = false;
            M2ModelIsOn = false;
            A2ModelIsOn = false;

            //zoom reset
            feild_of_vew = 70.0f;
            temp_feild_of_vew = 70.0f;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            // Camera parameters for view transform
            cameraPosition = glm::vec3(-5.2f, 1.0f, -15.5f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // flag H model
        {
            //reseting camera and world angle
            //cam x angle
            camx = 1.57;
            tempcamx = 1.57;
            //cam y angle
            camy = 0;
            tempcamy = 0;

            worldAnglex = 0;
            worldAnglex = 0;
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            CModelIsOn = false;
            HModelIsOn = true;
            A1ModelIsOn = false;
            M1ModelIsOn = false;
            M2ModelIsOn = false;
            A2ModelIsOn = false;

            //zoom reset
            feild_of_vew = 70.0f;
            temp_feild_of_vew = 70.0f;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            // Camera parameters for view transform
            cameraPosition = glm::vec3(-3.3f, 1.0f, -15.5f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // flag A model
        {
            //reseting camera and world angle
            //cam x angle
            camx = 1.57;
            tempcamx = 1.57;
            //cam y angle
            camy = 0;
            tempcamy = 0;

            worldAnglex = 0;
            worldAnglex = 0;
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            CModelIsOn = false;
            HModelIsOn = false;
            A1ModelIsOn = true;
            M1ModelIsOn = false;
            M2ModelIsOn = false;
            A2ModelIsOn = false;

            //zoom reset
            feild_of_vew = 70.0f;
            temp_feild_of_vew = 70.0f;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            // Camera parameters for view transform
            cameraPosition = glm::vec3(-1.0f, 1.0f, -15.5f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // flag M model
        {
            //reseting camera and world angle
            //cam x angle
            camx = 1.57;
            tempcamx = 1.57;
            //cam y angle
            camy = 0;
            tempcamy = 0;

            worldAnglex = 0;
            worldAnglex = 0;
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            CModelIsOn = false;
            HModelIsOn = false;
            A1ModelIsOn = false;
            M1ModelIsOn = true;
            M2ModelIsOn = false;
            A2ModelIsOn = false;

            //zoom reset
            feild_of_vew = 70.0f;
            temp_feild_of_vew = 70.0f;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            // Camera parameters for view transform
            cameraPosition = glm::vec3(1.5f, 1.0f, -15.5f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) // flag M model
        {
            //reseting camera and world angle
            //cam x angle
            camx = 1.57;
            tempcamx = 1.57;
            //cam y angle
            camy = 0;
            tempcamy = 0;

            worldAnglex = 0;
            worldAnglex = 0;
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            CModelIsOn = false;
            HModelIsOn = false;
            A1ModelIsOn = false;
            M1ModelIsOn = false;
            M2ModelIsOn = true;
            A2ModelIsOn = false;

            //zoom reset
            feild_of_vew = 70.0f;
            temp_feild_of_vew = 70.0f;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            // Camera parameters for view transform
            cameraPosition = glm::vec3(4.0f, 1.0f, -15.5f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) // flag A model
        {
            //cam x angle
            camx = 1.57;
            tempcamx = 1.57;
            //cam y angle
            camy = 0;
            tempcamy = 0;

            worldAnglex = 0;
            worldAnglex = 0;
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            CModelIsOn = false;
            HModelIsOn = false;
            A1ModelIsOn = false;
            M1ModelIsOn = false;
            M2ModelIsOn = false;
            A2ModelIsOn = true;

            //zoom reset
            feild_of_vew = 70.0f;
            temp_feild_of_vew = 70.0f;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            // Camera parameters for view transform
            cameraPosition = glm::vec3(6.0f, 1.0f, -15.5f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }


        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) // unflag all models
        {
            CModelIsOn = false;
            HModelIsOn = false;
            A1ModelIsOn = false;
            M1ModelIsOn = false;
            M2ModelIsOn = false;
            A2ModelIsOn = false;
        }

        //changing between the points, lines and triangles functionality
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            draw = GL_LINE_STRIP;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            draw = GL_POINTS;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            draw = GL_TRIANGLES;
        }

        //Arrow keys for world rotation
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            worldAnglex = (worldAnglex + rotationSpeed * dt); // angles in degrees, but glm expects radians (conversion below)
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            worldAngley = (worldAngley + rotationSpeed * dt); // angles in degrees, but glm expects radians (conversion below)
        }if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            worldAngley = (worldAngley - rotationSpeed * dt); // angles in degrees, but glm expects radians (conversion below)
        }if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            worldAnglex = (worldAnglex - rotationSpeed * dt); // angles in degrees, but glm expects radians (conversion below)
        }

        //reseting the camera with the home button
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
        {
            //cam x angle
            camx = 1.57;
            tempcamx = 1.57;
            //cam y angle
            camy = 0;
            tempcamy = 0;
            // Camera parameters for view transform
            worldAnglex = 0;
            worldAngley = 0;
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            //zoom reset
            feild_of_vew = 70.0f;
            temp_feild_of_vew = 70.0f;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            // Camera parameters for view transform
            cameraPosition = glm::vec3(0.6f, 1.0f, 10.0f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        //reseting the camera only with O
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            // Camera parameters for view transform
            cameraPosition = glm::vec3(0.6f, 1.0f, 10.0f);
            cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }


        //using mouse for tilting, panning and zooming
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            camy = tempcamy + (ypos - tempypos) / 500;
            camx = tempcamx + (xpos - tempxpos) / 500;
            cameraLookAt = glm::vec3(cosf(camy) * cosf(camx)
                , sinf(camy)
                , -cosf(camy) * sinf(camx));
            //tempxpos = xpos;
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            if (temp_feild_of_vew + ((ypos - tempypos) / 100) < 70.0f && temp_feild_of_vew + ((ypos - tempypos) / 100) > 69.2f) {
                feild_of_vew = temp_feild_of_vew + ((ypos - tempypos) / 100);
            }

            //tempxpos = xpos;
            glm::mat4 projectionMatrix = glm::perspective(feild_of_vew,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


        }
        else {
            glfwGetCursorPos(window, &tempxpos, &tempypos);
            tempcamx = camx;
            tempcamy = camy;
            temp_feild_of_vew = feild_of_vew;
        }

        glm::mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}