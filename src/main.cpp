#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

#include "game.hpp"
#include "resource_manager.hpp"

// settings
const unsigned int screenWidth { 800 };
const unsigned int screenHeight { 600 };

Game Breakout { screenWidth, screenHeight };
ResourceManager resourceManager;

// camera
bool isFirstMouse { true };
double lastX { screenWidth / 2.0f };
double lastY { screenHeight / 2.0f };

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mode);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

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
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Game Engine", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Breakout.init(resourceManager);

    // timing
    float deltaTime { 0.0f }; // Time between current frame and last frame
    float lastFrame { 0.0f }; // Time of last frame

    Breakout.setState(Menu);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        float current_frame { static_cast<float>(glfwGetTime()) };
        deltaTime = current_frame - lastFrame;
        lastFrame = current_frame;

        // input
        // -----
        Breakout.processInput(deltaTime);

        Breakout.update(deltaTime);

        // render
        // ------
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.render(resourceManager);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    resourceManager.clear();

    // glfw: terminate, clearing all previously allocated GLFW resources
    // -----------------------------------------------------------------
    glfwTerminate();
    return EXIT_SUCCESS;
}

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            Breakout.setKey(key, true);
        else if (action == GLFW_RELEASE)
            Breakout.setKey(key, false);
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
