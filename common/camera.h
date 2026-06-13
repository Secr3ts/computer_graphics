#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

using namespace glm;

class Camera {
public:
    Camera(void);
    ~Camera(void);

    float theta;
    float phi;
    float zoom;

    bool initial;

    vec3 position;

    mat4 view;
    mat4 proj;

    void Init(float xpos, float ypos);
    void Position(float xpos, float ypos);
    void Zoom(float ypos);
    
    void CalculateView();
    void CalculateProj();
private:
    float last_x;
    float last_y;
};
#endif