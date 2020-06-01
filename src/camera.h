#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific
// input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for
// use in OpenGL
class Camera {
  public:
    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f,
           float pitch = 0.0f)
        : position{position}, world_up{up}, yaw{yaw}, pitch{pitch} {
        update_camera_vectors();
    }

    // constructor with scalar values
    Camera(float x_pos, float y_pos, float z_pos, float x_up, float y_up, float z_up, float yaw, float pitch)
        : position{glm::vec3(x_pos, y_pos, z_pos)}, world_up{glm::vec3(x_up, y_up, z_up)}, yaw{yaw}, pitch{pitch} {
        update_camera_vectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 get_view_matrix() { return glm::lookAt(this->position, this->position + this->front, this->up); }

    float get_zoom() { return this->zoom; }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera
    // defined ENUM (to abstract it from windowing systems)
    void process_keyboard(Camera_Movement direction, float delta_time) {
        float velocity{this->movement_speed * delta_time};
        if (direction == FORWARD) this->position += this->front * velocity;
        if (direction == BACKWARD) this->position -= this->front * velocity;
        if (direction == LEFT) this->position -= this->right * velocity;
        if (direction == RIGHT) this->position += this->right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch = true) {
        x_offset *= this->mouse_sensitivity;
        y_offset *= this->mouse_sensitivity;

        this->yaw += x_offset;
        this->pitch += y_offset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        this->pitch = constrain_pitch ? glm::clamp(this->pitch, -89.0f, 89.0f) : this->pitch;

        // update Front, Right and Up Vectors using the updated Euler angles
        update_camera_vectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void process_mouse_scroll(float yoffset) {
        this->zoom -= (float)yoffset;
        this->zoom = glm::clamp(this->zoom, 1.0f, 90.0f);
    }

  private:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movement_speed{2.5f};
    float mouse_sensitivity{0.1f};
    float zoom{45.0f};

    // calculates the front vector from the Camera's (updated) Euler Angles
    void update_camera_vectors() {
        // calculate the new Front vector
        glm::vec3 front = {cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                           sin(glm::radians(this->pitch)),
                           sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))};
        this->front     = glm::normalize(front);
        // also re-calculate the Right and Up vector
        this->right = glm::normalize(glm::cross(this->front, this->world_up));
        // the more you look up or down which results in slower movement.
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif