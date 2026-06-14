#pragma once
#include "GLShader.h"
#include <GL/gl.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <sys/types.h>
#include <vector>
#include "tiny_obj_loader.h"

#ifndef LOADER_H
#define LOADER_H

using namespace std;
using namespace glm;

struct InstanceData {
  mat4 m_matrix;
  vec3 m_amb;
  vec3 m_dif;
  vec3 m_spec;
  float m_shine; 
}; // Hardware instancing !

struct Vertex {
  float position[3];
  float normal[3];
  float texcoord[2];
};

class Model {
public:
  vector<Vertex> vertices;
  vector<uint> indices;
  vector<InstanceData> instance_data;
  vector<tinyobj::material_t> materials;

  string name;
  size_t instances;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  GLuint iVBO; // Instance VBO

  GLShader shader;

  Model(void);
  ~Model(void);

  void Load(string obj,string mtl = "./", string mtl_name = "");
  void LoadShader(string path, string name);
  
  void Upload();
  
  void SetInstanceTransforms(const vector<mat4> &transforms);
  void SetInstanceTransforms(InstanceData* instance, const vector<mat4> &transforms);
  void SetInstanceData(const vector<InstanceData> &data);

  void SetMaterial(InstanceData* instance, const tinyobj::material_t &material);
  
  void Draw(const mat4 &viewMtx, const mat4 &projMtx, const vec3 &cameraPos);

  // ALWAYS USE Sc, THEN Tr.
  void Tr(vec3 offset);
  // ALWAYS USE Sc, THEN Tr.
  void Sc(vec3 factor);

  void GenerateBuffers();
  void DestroyBuffers();
};
#endif