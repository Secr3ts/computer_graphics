#include "camera.h"
#include "app_config.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

using namespace std;
using namespace glm;

Camera::Camera(void): theta(90.0f), phi(0.0f), zoom(45.0f), initial(true), position(vec3(0.0f, 0.0f, 5.0f)), view(mat4()), proj(mat4()), last_x(APP_WINDOW_WIDTH * 0.5f), last_y(APP_WINDOW_HEIGHT * 0.5f) {}

Camera::~Camera(void) {}

void Camera::Init(float xpos, float ypos) {
    last_x = xpos;
    last_y = ypos;
}

void Camera::Position(float xpos, float ypos) {
    float xoff = xpos - last_x;
    float yoff = ypos - last_y;

    Init(xpos, ypos);

    float s = 0.2f;

    theta += xoff * s;
    phi += yoff * s;

    // Avoid edge cases
    if (phi > 89.0f) {
        phi = 89.0f;
    }
    
    if (phi < -89.0f) {
        phi = -89.0f;
    }
}

void Camera::Zoom(float ypos) {
    zoom -= (float)ypos;

    // edge cases
    if (zoom < 1.0f) {
        zoom = 1.0f;
    }

    if (zoom > 45.0f * 2.f) {
        zoom = 45.0f * 2.f;
    }
}

void Camera::CalculateView() {
    vec3 f;
    f.x = cos(radians(theta)) * cos(radians(phi));
    f.y = sin(radians(phi));
    f.z = sin(radians(theta)) * cos(radians(phi));

    const float radius = 5.0f;
    position = vec3(radius * f.x, radius * f.y, radius * f.z);

    view = lookAt(position, vec3(0.0f, 0.0f, 0.0f), vec3(0.f, 1.f, 0.f));
}

void Camera::CalculateProj() {
    proj = perspective(radians(zoom), static_cast<float>(APP_WINDOW_WIDTH) / static_cast<float>(APP_WINDOW_HEIGHT), 0.1f, 100.0f);
}