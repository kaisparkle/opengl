#include <SDL.h>
#include "camera.h"

FlyCamera::FlyCamera(float fovDeg, float aspect, float near, float far) {
    update_projection_matrix(fovDeg, aspect, near, far);
    position = POSITION_DEFAULT;
    _pitch = PITCH_DEFAULT;
    _yaw = YAW_DEFAULT;
    _velocity = VELOCITY_DEFAULT;
    _sensitivity = SENSITIVITY_DEFAULT;
    _up = glm::vec3(0.0f, 1.0f, 0.0f);
    _front = glm::vec3(0.0f, 0.0f, -1.0f);
    _worldUp = _up;
    update_camera_vectors();
}

void FlyCamera::update_projection_matrix(float fovDeg, float aspect, float near, float far) {
    _fovDeg = fovDeg;
    _aspect = aspect;
    _near = near;
    _far = far;
    projection = glm::perspective(glm::radians(_fovDeg), _aspect, _near, _far);
}

glm::mat4 FlyCamera::get_view_matrix() {
    return glm::lookAt(position, position + _front, _up);
}

void FlyCamera::process_keyboard(double delta, const uint8_t *keystate) {
    auto velocity = (float) (_velocity * (delta / 1000));
    if (keystate[SDL_SCANCODE_W]) position += _front * velocity;
    if (keystate[SDL_SCANCODE_A]) position -= _right * velocity;
    if (keystate[SDL_SCANCODE_S]) position -= _front * velocity;
    if (keystate[SDL_SCANCODE_D]) position += _right * velocity;
    if (keystate[SDL_SCANCODE_Q]) position -= _up * velocity;
    if (keystate[SDL_SCANCODE_E]) position += _up * velocity;
}

void FlyCamera::process_mouse(float dx, float dy) {
    dx *= _sensitivity;
    dy *= _sensitivity;
    _yaw += dx;
    _pitch += -dy;

    if (_pitch > 89.0f) _pitch = 89.0f;
    if (_pitch < -89.0f) _pitch = -89.0f;

    update_camera_vectors();
}

void FlyCamera::update_camera_vectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
}
