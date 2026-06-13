#pragma once
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#ifndef FINAL_H
#define FINAL_H
#include "common/app_config.h"
#include "common/Vector3.h"
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION

bool Initialize();
void Terminate();
void Display();
void Render();

GLuint LoadCubemap(const std::vector<std::string> &faces);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int main(int argc, char *argv[]);

#endif