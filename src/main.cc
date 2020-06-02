#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include <iostream>

#include "camera.h"
#include "model.h"
#include "shader.h"

// settings
const unsigned int SCR_WIDTH{800};
const unsigned int SCR_HEIGHT{600};

// camera
Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
bool   first_mouse{true};
double last_x{SCR_WIDTH / 2.0f};
double last_y{SCR_HEIGHT / 2.0f};

// timing
float delta_time{0.0f}; // Time between current frame and last frame
float last_frame{0.0f}; // Time of last frame

// lighting
glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

GLuint loadTexture(char const* path);
void   framebuffer_size_callback(GLFWwindow* window, int width, int height);
void   scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void   mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void   process_input(GLFWwindow* window);

int main() {
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Game Engine", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader shader{"model_loading.vert", "model_loading.frag"};

    Model model{"backpack/backpack.obj"};

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        float current_frame{static_cast<float>(glfwGetTime())};
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        // -----
        process_input(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // active shader

        shader.use();
        glm::mat4 projection{glm::perspective(glm::radians(camera.get_zoom()),
                                              static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f,
                                              100.0f)};
        glm::mat4 view{camera.get_view_matrix()};
        shader.set_mat4("projection", projection);
        shader.set_mat4("view", view);

        glm::mat4 model_matrix{glm::mat4(1.0f)};
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.0f));
        shader.set_mat4("model", model_matrix);
        model.draw(shader);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources
    // -----------------------------------------------------------------
    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) { camera.process_mouse_scroll(y_offset); }

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    if (first_mouse) {
        last_x      = x_pos;
        last_y      = y_pos;
        first_mouse = false;
    }

    double x_offset{x_pos - last_x};
    double y_offset{last_y - y_pos}; // reversed since y-coordinates range from bottom to top

    last_x = x_pos;
    last_y = y_pos;

    camera.process_mouse_movement(x_offset, y_offset);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.process_keyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.process_keyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.process_keyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.process_keyboard(RIGHT, delta_time);
}