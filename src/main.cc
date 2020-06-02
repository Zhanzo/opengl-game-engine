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

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    Shader lighting_shader{"lighting.vert", "lighting.frag"};
    Shader light_cube_shader{"lighting.vert", "light_cube.frag"};

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f};
    glm::vec3 cubePositions[]       = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                 glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                 glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                 glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
    glm::vec3 pointLightPositions[] = {glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
                                       glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

    GLuint VBO, cube_VAO, light_cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attribute(s).
    glBindVertexArray(cube_VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &light_cube_VAO);
    glBindVertexArray(light_cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::string path{"container2.png"};
    GLuint      diffuse_map{loadTexture(path.c_str())};

    path = "container2_specular.png";
    GLuint specular_map{loadTexture(path.c_str())};

    lighting_shader.use();
    lighting_shader.set_int("material.diffuse", 0);
    lighting_shader.set_int("material.specular", 1);

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
            lighting_shader.use();
            lighting_shader.set_float("material.shininess", 32.0f);
            lighting_shader.set_vec3("view_pos", camera.get_position());

            // directional light
            lighting_shader.set_vec3("dir_light.direction", -0.2f, -1.0f, -0.3f);
            lighting_shader.set_vec3("dir_light.ambient", 0.05f, 0.05f, 0.05f);
            lighting_shader.set_vec3("dir_light.diffuse", 0.4f, 0.4f, 0.4f);
            lighting_shader.set_vec3("dir_light.specular", 0.5f, 0.5f, 0.5f);
            // point light 1
            lighting_shader.set_vec3("point_lights[0].position", pointLightPositions[0]);
            lighting_shader.set_vec3("point_lights[0].ambient", 0.05f, 0.05f, 0.05f);
            lighting_shader.set_vec3("point_lights[0].diffuse", 0.8f, 0.8f, 0.8f);
            lighting_shader.set_vec3("point_lights[0].specular", 1.0f, 1.0f, 1.0f);
            lighting_shader.set_float("point_lights[0].constant", 1.0f);
            lighting_shader.set_float("point_lights[0].linear", 0.09);
            lighting_shader.set_float("point_lights[0].quadratic", 0.032);
            // point light 2
            lighting_shader.set_vec3("point_lights[1].position", pointLightPositions[1]);
            lighting_shader.set_vec3("point_lights[1].ambient", 0.05f, 0.05f, 0.05f);
            lighting_shader.set_vec3("point_lights[1].diffuse", 0.8f, 0.8f, 0.8f);
            lighting_shader.set_vec3("point_lights[1].specular", 1.0f, 1.0f, 1.0f);
            lighting_shader.set_float("point_lights[1].constant", 1.0f);
            lighting_shader.set_float("point_lights[1].linear", 0.09);
            lighting_shader.set_float("point_lights[1].quadratic", 0.032);
            // point light 3
            lighting_shader.set_vec3("point_lights[2].position", pointLightPositions[2]);
            lighting_shader.set_vec3("point_lights[2].ambient", 0.05f, 0.05f, 0.05f);
            lighting_shader.set_vec3("point_lights[2].diffuse", 0.8f, 0.8f, 0.8f);
            lighting_shader.set_vec3("point_lights[2].specular", 1.0f, 1.0f, 1.0f);
            lighting_shader.set_float("point_lights[2].constant", 1.0f);
            lighting_shader.set_float("point_lights[2].linear", 0.09);
            lighting_shader.set_float("point_lights[2].quadratic", 0.032);
            // point light 4
            lighting_shader.set_vec3("point_lights[3].position", pointLightPositions[3]);
            lighting_shader.set_vec3("point_lights[3].ambient", 0.05f, 0.05f, 0.05f);
            lighting_shader.set_vec3("point_lights[3].diffuse", 0.8f, 0.8f, 0.8f);
            lighting_shader.set_vec3("point_lights[3].specular", 1.0f, 1.0f, 1.0f);
            lighting_shader.set_float("point_lights[3].constant", 1.0f);
            lighting_shader.set_float("point_lights[3].linear", 0.09);
            lighting_shader.set_float("point_lights[3].quadratic", 0.032);

            lighting_shader.set_mat4("projection", projection);
            lighting_shader.set_mat4("view", view);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuse_map);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specular_map);

            glBindVertexArray(cube_VAO);
            for (unsigned int i{0}; i < 10; ++i) {
                glm::mat4 model{glm::mat4(1.0f)};
                model = glm::translate(model, cubePositions[i]);
                model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
                lighting_shader.set_mat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        {
            light_cube_shader.use();
            light_cube_shader.set_mat4("projection", projection);
            light_cube_shader.set_mat4("view", view);

            glBindVertexArray(light_cube_VAO);
            for (unsigned int i{0}; i < 4; i++) {
                glm::mat4 model{glm::mat4(1.0f)};
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f));
                light_cube_shader.set_mat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
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

// utility function for loading a 2D texture from file
// ---------------------------------------------------
GLuint loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int            width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
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