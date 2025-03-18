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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 55.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LAB5", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader1("sahne.vs", "sahne.fs");
    Shader shader2("eksen.vs", "eksen.fs");

    // Define NUM_OF_POINTS and VAO
    const int NUM_OF_POINTS = 3;

    // Create vertex data for axis lines
    float axisVertices[] = {
        // Position
        0.0f, 0.0f, 0.0f, // origin
        100.0f, 0.0f, 0.0f, // x-axis end

        0.0f, 0.0f, 0.0f, // origin
        0.0f, 100.0f, 0.0f, // y-axis end

        0.0f, 0.0f, 0.0f, // origin
        0.0f, 0.0f, 100.0f  // z-axis end
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);   

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
   

    // load models
    // -----------
    Model planet(FileSystem::getPath("resources/lab5/planet/planet.obj"));
    Model rock(FileSystem::getPath("resources/lab5/rock/rock.obj"));

    unsigned int texture1 = TextureFromFile("green.png", FileSystem::getPath("resources/lab5").c_str());
    Model teapot_green(FileSystem::getPath("resources/lab5/teapot/teapot.obj"));

    unsigned int texture2 = TextureFromFile("spots.png", FileSystem::getPath("resources/lab5").c_str());
    Model teapot_spots(FileSystem::getPath("resources/lab5/teapot/teapot.obj"));

    unsigned int texture3 = TextureFromFile("wood_dark.png", FileSystem::getPath("resources/lab5").c_str());
    Model teapot_wood(FileSystem::getPath("resources/lab5/teapot/teapot.obj"));

    unsigned int texture4 = TextureFromFile("top.jpg", FileSystem::getPath("resources/lab5").c_str());
    Model teapot_top(FileSystem::getPath("resources/lab5/teapot/teapot.obj"));




    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int amount = 7;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    //float radius = 50.0;
    //float offset = 2.5f;

    //1-) x-y-z eksenlerinde 0.5 ölçekleme ve (-10, -10, -10) kaydırma ile çaydanlık modelini yeşil dokuda çizdirme
    glm::mat4 model0 = glm::mat4(1.0f);
    model0 = glm::translate(model0, glm::vec3(-10.0, -10.0, -10.0));
    model0 = glm::scale(model0, glm::vec3(0.5, 0.5, 0.5));
    modelMatrices[0] = model0;


    // 2-) x-y-z eksenlerinde 0.05 ölçekleme ve (-10, -10, -10) kaydırma ile gezegen modelini orijinal dokusunda çizdirme
    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, glm::vec3(-10.0f, -10.0f, -10.0f));
    model1 = glm::scale(model1, glm::vec3(0.05f, 0.05f, 0.05f));
    modelMatrices[1] = model1;

    // 3-) Başlangıç konumu (10, 10, 10) olan y eksenine parallel bir eksen etrafında 150° döndürme ile çaydanlık modelini benekli dokuda çizdirme
    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, glm::vec3(10.0f, 10.0f, 10.0f));
    float rotAngle = 150.0f;
    model2 = glm::rotate(model2, glm::radians(rotAngle), glm::vec3(0, 1, 0));
    modelMatrices[2] = model2;

    //4-) x-y-z eksenlerinde 0.05 ölçekleme ve (10, 10, 10) kaydırma ile gezegen modelini orijinal dokusunda çizdirme
    glm::mat4 model3 = glm::mat4(1.0f);
    model3 = glm::translate(model3, glm::vec3(10.0f, 10.0f, 10.0f));
    model3 = glm::scale(model3, glm::vec3(0.05f, 0.05f, 0.05f));
    modelMatrices[3] = model3;

    //5-) (-10, -10, -10) sabit noktasına göre x-y-z eksenlerinde sırasıyla (0.7, 0.3, 0.3) ölçekleme ile çaydanlık modelini koyu ahşap dokuda çizdirme
    glm::mat4 model4 = glm::mat4(1.0f);
    model4 = glm::translate(model4, glm::vec3(-10.0f, -10.0f, -10.0f));
    model4 = glm::scale(model4, glm::vec3(0.7f, 0.3f, 0.3f));
    model4 = glm::translate(model4, glm::vec3(10.0f, 10.0f, 10.0f));
    modelMatrices[4] = model4;

    //6-) (-1, -1, -1) orijin noktalı ve eksenleri x-y-z eksenleriyle paralel x’,y’,z’ koordinat sistemine göre x-y düzleminde yansıtma ile çaydanlık modelini koyu mavi (top) dokuda çizdirme
    glm::mat4 model5 = glm::mat4(1.0f);
    model5 = glm::translate(model5, glm::vec3(-1.0f, -1.0f, -1.0f));
    model5 = glm::scale(model5, glm::vec3(1.0f, 1.0f, -1.0f));
    model5 = glm::translate(model5, glm::vec3(1.0f, 1.0f, 1.0f));
    modelMatrices[5] = model5;

    //7-) x-y-z eksenlerinde 0.05 ölçekleme ve (-1, -1, -1) kaydırma ile kaya modelini orijinal dokusunda çizdirme
    glm::mat4 model6 = glm::mat4(1.0f);
    model6 = glm::translate(model6, glm::vec3(-1.0f, -1.0f, -1.0f));
    model6 = glm::scale(model6, glm::vec3(0.05f, 0.05f, 0.05f));
    modelMatrices[6] = model6;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();;
        shader1.use();
        shader1.setMat4("projection", projection);
        shader1.setMat4("view", view);

        //çaydanlık modelini yeşil dokuda çizdirme
        glBindTexture(GL_TEXTURE_2D, texture1);
        shader1.setMat4("model", modelMatrices[0]);
        teapot_green.Draw(shader1);

        // draw planet
        shader1.setMat4("model", modelMatrices[1]);
        planet.Draw(shader1);

        // çaydanlık modelini benekli dokuda çizdirme
        glBindTexture(GL_TEXTURE_2D, texture2);
        shader1.setMat4("model", modelMatrices[2]);
        teapot_spots.Draw(shader1);

        // draw planet
        shader1.setMat4("model", modelMatrices[3]);
        planet.Draw(shader1);

        // çaydanlık modelini koyu ahşap dokuda çizdirme
        glBindTexture(GL_TEXTURE_2D, texture3);
        shader1.setMat4("model", modelMatrices[4]);
        teapot_wood.Draw(shader1);

        //çaydanlık modelini koyu mavi (top) dokuda çizdirme
        glBindTexture(GL_TEXTURE_2D, texture4);
        shader1.setMat4("model", modelMatrices[5]);
        teapot_top.Draw(shader1);

        shader1.setMat4("model", modelMatrices[6]);
        rock.Draw(shader1);

        
        shader2.use();
        shader2.setMat4("projection", projection);
        shader2.setMat4("view", view);
        glm::mat4 model_axis = glm::mat4(1.0f);

        // x axis rendering in red
        model_axis = glm::mat4(1.0f);
        shader2.setMat4("model", model_axis);
        shader2.setVec4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, NUM_OF_POINTS);

        // y axis rendering in green
        model_axis = glm::rotate(model_axis, glm::radians(90.0f), glm::vec3(0, 0, 1));
        shader2.setMat4("model", model_axis);
        shader2.setVec4("ourColor", 0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, NUM_OF_POINTS);

        // z axis rendering in blue
        model_axis = glm::mat4(1.0f);
        model_axis = glm::rotate(model_axis, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        shader2.setMat4("model", model_axis);
        shader2.setVec4("ourColor", 0.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, NUM_OF_POINTS);




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
