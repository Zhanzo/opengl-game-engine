#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "camera.h"
#include "shader.h"
#include "stb_image.h"

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void process_input(GLFWwindow* window);

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

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    Shader lighting_shader{"lighting.vert", "lighting.frag"};
    Shader light_cube_shader{"lighting.vert", "light_cube.frag"};

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    glm::vec3 vertices[] = {
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), // first
        glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f), // second
        glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(0.0f, 0.0f, -1.0f), // third
        glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(0.0f, 0.0f, -1.0f), // fourth
        glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f), // fifth
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), // sixth

        glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f), // first
        glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(0.0f, 0.0f, 1.0f), // second
        glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.0f, 0.0f, 1.0f), // third
        glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.0f, 0.0f, 1.0f), // fourth
        glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(0.0f, 0.0f, 1.0f), // fifth
        glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f), // sixth

        glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-1.0f, 0.0f, 0.0f), // first
        glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(-1.0f, 0.0f, 0.0f), // second
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), // third
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), // fourth
        glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-1.0f, 0.0f, 0.0f), // fifth
        glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-1.0f, 0.0f, 0.0f), // sixth

        glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(1.0f, 0.0f, 0.0f), // first
        glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(1.0f, 0.0f, 0.0f), // second
        glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f, 0.0f, 0.0f), // third
        glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f, 0.0f, 0.0f), // fourth
        glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(1.0f, 0.0f, 0.0f), // fifth
        glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(1.0f, 0.0f, 0.0f), // sixth

        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), // first
        glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f, 0.0f), // second
        glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(0.0f, -1.0f, 0.0f), // third
        glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(0.0f, -1.0f, 0.0f), // fourth
        glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, -1.0f, 0.0f), // fifth
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), // sixth

        glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f), // first
        glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(0.0f, 1.0f, 0.0f), // second
        glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.0f, 1.0f, 0.0f), // third
        glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.0f, 1.0f, 0.0f), // fourth
        glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(0.0f, 1.0f, 0.0f), // fifth
        glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f)  // sixth
    };

    GLuint VBO, cube_VAO, light_cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attribute(s).
    glBindVertexArray(cube_VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(light_cube_VAO);
    glGenVertexArrays(1, &light_cube_VAO);
    glBindVertexArray(light_cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // active shader

        glm::mat4 projection{glm::perspective(glm::radians(camera.get_zoom()),
                                              static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f,
                                              100.0f)};
        glm::mat4 view{camera.get_view_matrix()};

        light_pos = glm::vec3(1.5f * sin(glfwGetTime()), 0.0f, 1.5f * cos(glfwGetTime()));

        {
            glm::mat4 model{glm::mat4(1.0f)};

            lighting_shader.use();
            lighting_shader.set_vec3("object_color", 1.0f, 0.5f, 0.31f);
            lighting_shader.set_vec3("light_color", 1.0f, 1.0f, 1.0f);
            lighting_shader.set_vec3("light_pos", light_pos);
            lighting_shader.set_float("exp", 128.0f);
            lighting_shader.set_vec3("view_pos", camera.get_position());
            lighting_shader.set_mat4("model", model);
            lighting_shader.set_mat4("projection", projection);
            lighting_shader.set_mat4("view", view);

            glBindVertexArray(cube_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        {
            glm::mat4 model{glm::mat4(1.0f)};
            model = glm::translate(model, light_pos);
            model = glm::scale(model, glm::vec3(0.2f));

            light_cube_shader.use();
            light_cube_shader.set_mat4("model", model);
            light_cube_shader.set_mat4("projection", projection);
            light_cube_shader.set_mat4("view", view);

            glBindVertexArray(light_cube_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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