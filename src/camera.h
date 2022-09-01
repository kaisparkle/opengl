#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// defaults
const glm::vec3 POSITION_DEFAULT = glm::vec3(0.0f, 0.0f, 0.0f);
const float PITCH_DEFAULT = 0.0f;
const float YAW_DEFAULT = -90.0f;
const float VELOCITY_DEFAULT = 20.0f;
const float SENSITIVITY_DEFAULT = 0.1f;

class FlyCamera {
public:
    FlyCamera(float fovDeg, float aspect, float near, float far);

    void update_projection_matrix(float fovDeg, float aspect, float near, float far);

    glm::mat4 get_view_matrix();

    void process_keyboard(double delta, const uint8_t *keystate);

    void process_mouse(float dx, float dy);

    glm::mat4 projection;
    glm::vec3 position;
private:
    float _fovDeg = 0;
    float _aspect = 0;
    float _near = 0;
    float _far = 0;
    glm::vec3 _up;
    glm::vec3 _front;
    glm::vec3 _right;
    glm::vec3 _worldUp;
    float _pitch;
    float _yaw;
    float _velocity;
    float _sensitivity;

    void update_camera_vectors();
};