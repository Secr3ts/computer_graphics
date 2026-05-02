#pragma once
#include <GLFW/glfw3.h>
#ifndef FINAL_H
#define FINAL_H
#include "common/Vector3.h"

#define W_WIDTH 1080
#define W_HEIGHT 720

int main(int argc, char **argv);

// Exercice 1.1
// Column-order matrixes !
float *matmul4x4(float *m1, float *m2);

// Exercice 1.2 Normals
float *calculateNormals3();

// TP à rendre
void updateViewMatrix();
void updateWorldMatrix(float angleX, float angleY, float angleZ);
template <typename T>
Vector3<T> toCartesian();

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// Exercice 2.1 LookAt
float *LookAt(Vector3<float> position, Vector3<float> target, Vector3<float> up);

// Debugging
static void printMatrix(const char* name, float *m);

// OpenGL Related functions
bool Initialize();
void Terminate();
void Display();
void Render();

#endif