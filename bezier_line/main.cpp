#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>

#define NUM_OF_POINTS 30

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 20.0f, 60.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void binomialCoefficients(GLint n, GLint* C);
void computeBezierPoint(GLfloat u, glm::vec3* bezierPoints, GLint numOfControlPoints, const glm::vec4* controlPoints, const GLint* C);
void bezierSpline(GLint numOfControlPoints, const glm::vec4* controlPoints, GLint numOfBezierSplinePoints, GLfloat* bezierSplinePoints);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef APPLE
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_VISIBLE, false);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "lab6", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 500, 200);
    glfwShowWindow(window);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader1("sahne.vs", "sahne.fs");
    Shader shader2("eksen.vs", "eksen.fs");

    // Load helicopter model
    Model helicopter(FileSystem::getPath("resources/Lab6/helecopter/chopper.obj"));

    glm::mat4 modelMatrix;
    glm::mat4 model_axis;
    // Define vertices for lines in coordinate system
    float vertices[NUM_OF_POINTS * 3];
    for (int i = 0; i < NUM_OF_POINTS; i++)
    {
        vertices[3 * i] = i;
        vertices[3 * i + 1] = 0;
        vertices[3 * i + 2] = 0;
    }
    // Array to hold Bezier Curve points
    float route[NUM_OF_POINTS * 100 * 3];

    // Define control points for Bezier Curve
    GLint numOfControlPoints = 5;
    GLint numOfBezierSplinePoints = NUM_OF_POINTS * 100;
    GLint scale = 5;
    glm::vec4 controlPoints[5] = {
        {5.0f, 5.0f, 5.0f, 1.0f},
        {5.0f, -8.0f, -10.0f, 1.0f},
        {-7.0f, 8.0f, -15.0f, 1.0f},
        {-4.0f, -4.0f, 10.0f, 1.0f},
        {5.0f, 5.0f, 5.0f, 1.0f}
    };

    // Scale control points
    model_axis = glm::mat4(1.0f);
    model_axis = glm::scale(model_axis, glm::vec3(scale, scale, scale));
    for (int p = 0; p < numOfControlPoints; p++)
        controlPoints[p] = model_axis * controlPoints[p];
    // Compute Bezier Spline points
    bezierSpline(numOfControlPoints, controlPoints, numOfBezierSplinePoints, route);

    // Setup VAOs and VBOs for rendering
    unsigned int VBO[2], VAO[2];
    glGenVertexArrays(1, &VAO[0]);
    glGenBuffers(1, &VBO[0]);
    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &VAO[1]);
    glGenBuffers(1, &VBO[1]);
    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(route), route, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Simulation parameters
    GLfloat simulationCycle = 5000;
    GLfloat sleepTime = 100; // in milliseconds
    GLint position = 0, position_before, position_after, offset;

    float timeStart = static_cast<float>(glfwGetTime());

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate frame time 
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        // Clear the color and depth buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Compute projection and view matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        // render the helicopter model
        shader1.use();
        shader1.setMat4("projection", projection);
        shader1.setMat4("view", view);

        // Compute current position along Bezier curve
        float timeLasted = currentFrame - timeStart;
        offset = NUM_OF_POINTS / 5;
        position = static_cast<int>(glm::round(numOfBezierSplinePoints * timeLasted * 1000 / simulationCycle));
        position_before = position - offset;
        position_after = position + offset;
        position = position % numOfBezierSplinePoints;
        position_before = position_before % numOfBezierSplinePoints;
        position_after = position_after % numOfBezierSplinePoints;

        // Compute model matrix for helicopter
        modelMatrix = glm::mat4(1.0f);

        glm::vec3 pos(route[3 * position], route[3 * position + 1], route[3 * position + 2]);
        glm::vec3 pos_after(route[3 * position_after], route[3 * position_after + 1], route[3 * position_after + 2]);
        glm::vec3 pos_before(route[3 * position_before], route[3 * position_before + 1], route[3 * position_before + 2]);

        glm::vec3 vec = glm::normalize(pos_after - pos);
        glm::vec3 vec_yzref(0, vec.y, vec.z);
        GLfloat alpha = glm::atan(vec.y, vec.z);
        GLfloat beta = glm::atan(-vec.x, glm::length(vec_yzref));

        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix = glm::rotate(modelMatrix, -beta, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, -alpha, glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, -1.0f));

        shader1.setMat4("model", modelMatrix);
        helicopter.Draw(shader1);

        // Render coordinate system and Bezier Curve
        shader2.use();
        shader2.setMat4("projection", projection);
        shader2.setMat4("view", view);

        glBindVertexArray(VAO[0]);
        glLineWidth(2.0f);
        
        // Draw x-axis
        model_axis = glm::mat4(1.0f);
        shader2.setMat4("model", model_axis);
        shader2.setVec4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, NUM_OF_POINTS);

	 // Draw y-axis
        model_axis = glm::rotate(model_axis, glm::radians(90.0f), glm::vec3(0, 0, 1));
        shader2.setMat4("model", model_axis);
        shader2.setVec4("ourColor", 0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, NUM_OF_POINTS);
    
        // Draw z-axis
        model_axis = glm::mat4(1.0f);
        model_axis = glm::rotate(model_axis, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        shader2.setMat4("model", model_axis);
        shader2.setVec4("ourColor", 0.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, NUM_OF_POINTS);

        // Draw Bezier Curve
        glBindVertexArray(VAO[1]);
        model_axis = glm::mat4(1.0f);
        shader2.setMat4("model", model_axis);
        shader2.setVec4("ourColor", 0.5f, 0.9f, 0.8f, 1.0f);
        glLineWidth(4.0f);
        glDrawArrays(GL_LINE_STRIP, 0, numOfBezierSplinePoints);

        // Swap front and back buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Clean up and exit
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glfwTerminate();

    return 0;
}

void binomialCoefficients(GLint n, GLint* C) {
    for (GLint k = 0; k <= n; k++) {
        C[k] = 1;
        for (GLint j = n; j >= k + 1; j--)
            C[k] *= j;
        for (GLint j = n - k; j >= 2; j--)
            C[k] /= j;
    }
}

void computeBezierPoint(GLfloat u, glm::vec3* bezierPoints, GLint numOfControlPoints, const glm::vec4* controlPoints, const GLint* C) {
    GLint n = numOfControlPoints - 1;
    GLfloat bezierBasisFunction;
    bezierPoints->x = bezierPoints->y = bezierPoints->z = 0.0f;

    for (GLint k = 0; k < numOfControlPoints; k++) {
        bezierBasisFunction = C[k] * (GLfloat)pow(u, k) * (GLfloat)pow(1 - u, n - k);
        bezierPoints->x += controlPoints[k].x * bezierBasisFunction;
        bezierPoints->y += controlPoints[k].y * bezierBasisFunction;
        bezierPoints->z += controlPoints[k].z * bezierBasisFunction;
    }
}

void bezierSpline(GLint numOfControlPoints, const glm::vec4* controlPoints, GLint numOfBezierSplinePoints, GLfloat* bezierSplinePoints) {
    glm::vec3 bezierSplinePoint;
    GLfloat u;
    GLint* C = new GLint[numOfControlPoints];
    binomialCoefficients(numOfControlPoints - 1, C);

    for (GLint k = 0; k < numOfBezierSplinePoints; k++) {
        u = GLfloat(k) / GLfloat(numOfBezierSplinePoints);
        computeBezierPoint(u, &bezierSplinePoint, numOfControlPoints, controlPoints, C);
        bezierSplinePoints[k * 3] = bezierSplinePoint.x;
        bezierSplinePoints[k * 3 + 1] = bezierSplinePoint.y;
        bezierSplinePoints[k * 3 + 2] = bezierSplinePoint.z;
    }
    delete[] C;
}

void processInput(GLFWwindow* window)
{
    float speed = 5.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime * speed);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

