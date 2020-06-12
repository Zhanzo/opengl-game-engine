#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific
// input methods
enum CameraMovement { Forward,
    Backward,
    Left,
    Right };

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for
// use in OpenGL
class Camera {
public:
    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0), glm::vec3 up = glm::vec3(0, 1, 0), float yaw = -90,
        float pitch = 0)
        : m_position { position }
        , m_front { 0, 0, -1 }
        , m_worldUp { up }
        , m_yaw { yaw }
        , m_pitch { pitch }
        , m_movementSpeed { 2.5 }
        , m_mouseSensitivity { 0.1 }
        , m_zoom { 45 }
    {
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw, float pitch)
        : m_position { glm::vec3(xPos, yPos, zPos) }
        , m_front { 0, 0, -1 }
        , m_worldUp { glm::vec3(xUp, yUp, zUp) }
        , m_yaw { yaw }
        , m_pitch { pitch }
        , m_movementSpeed { 2.5 }
        , m_mouseSensitivity { 0.1 }
        , m_zoom { 45 }
    {
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() { return glm::lookAt(m_position, m_position + m_front, m_up); }

    glm::vec3 getPosition() { return m_position; }

    float getZoom() { return m_zoom; }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera
    // defined ENUM (to abstract it from windowing systems)
    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity { m_movementSpeed * deltaTime };
        if (direction == Forward)
            m_position += m_front * velocity;
        if (direction == Backward)
            m_position -= m_front * velocity;
        if (direction == Left)
            m_position -= m_right * velocity;
        if (direction == Right)
            m_position += m_right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xOffset, float yOffset, bool isPitchConstrained = true)
    {
        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        m_yaw += xOffset;
        m_pitch += yOffset;

        // make sure that when m_pitch is out of bounds, screen doesn't get flipped
        m_pitch = isPitchConstrained ? glm::clamp(m_pitch, -89.0f, 89.0f) : m_pitch;

        // update front, right and up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yOffset)
    {
        m_zoom -= (float)yOffset;
        m_zoom = glm::clamp(m_zoom, 1.0f, 90.0f);
    }

private:
    glm::vec3 m_position, m_front, m_up, m_right, m_worldUp;
    float m_yaw, m_pitch;
    float m_movementSpeed, m_mouseSensitivity, m_zoom;

    // calculates the front vector from the Camera's (m_up) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new front vector
        glm::vec3 front = { cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
            sin(glm::radians(m_pitch)),
            sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)) };
        m_front = glm::normalize(front);
        // also re-calculate the Right and up vector
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        // the more you look up or down which results in slower movement.
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
};
