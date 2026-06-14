// https://gemini.google.com/app/076a9ecf04a16dc5 TODO TOMORROW
#include <cstddef>
#include <cstring>
#include <glm/ext/matrix_float4x4.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <GL/glew.h>    // first always
#include <GL/glext.h>   // third
#include "model.h" // last
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>

using namespace std;
using namespace tinyobj;
using namespace glm;

/* beginning of instance helper */
static InstanceData MakeDefaultInstance() {
  InstanceData data{};
  data.m_matrix = mat4(1.0f);
  data.m_matrix = translate(data.m_matrix, vec3(-1.0f, -1.0f, -1.0f));
  data.m_amb = vec3(0.1f, 0.1f, 0.1f);
  data.m_dif = vec3(1.0f, 1.0f, 1.0f);
  data.m_spec = vec3(0.2f, 0.2f, 0.2f);
  data.m_shine = 32.0f;
  return data;
}
/* end of instance helper */

Model::Model(void) : instances(1), VAO(0), VBO(0), EBO(0), iVBO(0) {
  instance_data.push_back(MakeDefaultInstance());
}

Model::~Model(void) {}

void Model::Load(string obj,string mtl,string mtl_name) {
  /* beginning of obj loading */
  ObjReaderConfig reader_config;
  ObjReader reader;

  reader_config.mtl_search_path = mtl;
  reader_config.triangulate = true;

  if (!reader.ParseFromFile(obj, reader_config)) {
    if (!reader.Error().empty()) {
      cerr << "TinyObjReader E: " << reader.Error();
    }

    exit(EXIT_FAILURE);
  }

  if (!reader.Warning().empty()) {
    cerr << "TinyObjReader W: " << reader.Warning();
  }

  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  materials = reader.GetMaterials();
  const bool hasNormals = !attrib.normals.empty();

  if (!materials.empty()) {
    InstanceData loaded = instance_data.empty() ? MakeDefaultInstance() : instance_data[0];
    material_t material;

    if (mtl_name.empty())
    {
      material = materials.front();
    } else {
      bool found = false;
      for (auto mtl: materials)
      {
        if (mtl.name == mtl_name) {
          material = mtl;
          found = true;
          break;
        }
      }

      if (!found) {
        material = materials.front();
      }
    }

    SetMaterial(&loaded, material);

    // Some teaching .mtl files set Ka to zero, which can make the model
    // appear fully black when normals/light are imperfect.
    if (loaded.m_amb.x == 0.0f && loaded.m_amb.y == 0.0f && loaded.m_amb.z == 0.0f) {
      loaded.m_amb.x = 0.2f * (loaded.m_dif.x > 0.0f ? loaded.m_dif.x : 1.0f);
      loaded.m_amb.y = 0.2f * (loaded.m_dif.y > 0.0f ? loaded.m_dif.y : 1.0f);
      loaded.m_amb.z = 0.2f * (loaded.m_dif.z > 0.0f ? loaded.m_dif.z : 1.0f);
    }

    for (size_t i = 0; i < instance_data.size(); ++i) {
      SetMaterial(&instance_data[i], material);
    }

  }

  for (size_t s = 0; s < shapes.size(); s++) {
    size_t index_offset = 0;

    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
      float faceNormal[3] = {0.0f, 0.0f, 1.0f};

      if (!hasNormals && fv >= 3) {
        tinyobj::index_t i0 = shapes[s].mesh.indices[index_offset + 0];
        tinyobj::index_t i1 = shapes[s].mesh.indices[index_offset + 1];
        tinyobj::index_t i2 = shapes[s].mesh.indices[index_offset + 2];

        float p0[3] = {attrib.vertices[3 * size_t(i0.vertex_index) + 0],
                       attrib.vertices[3 * size_t(i0.vertex_index) + 1],
                       attrib.vertices[3 * size_t(i0.vertex_index) + 2]};
        float p1[3] = {attrib.vertices[3 * size_t(i1.vertex_index) + 0],
                       attrib.vertices[3 * size_t(i1.vertex_index) + 1],
                       attrib.vertices[3 * size_t(i1.vertex_index) + 2]};
        float p2[3] = {attrib.vertices[3 * size_t(i2.vertex_index) + 0],
                       attrib.vertices[3 * size_t(i2.vertex_index) + 1],
                       attrib.vertices[3 * size_t(i2.vertex_index) + 2]};

        float e1[3] = {p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]};
        float e2[3] = {p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2]};

        faceNormal[0] = e1[1] * e2[2] - e1[2] * e2[1];
        faceNormal[1] = e1[2] * e2[0] - e1[0] * e2[2];
        faceNormal[2] = e1[0] * e2[1] - e1[1] * e2[0];

        float len = std::sqrt(faceNormal[0] * faceNormal[0] +
                              faceNormal[1] * faceNormal[1] +
                              faceNormal[2] * faceNormal[2]);
        if (len > 0.0f) {
          faceNormal[0] /= len;
          faceNormal[1] /= len;
          faceNormal[2] /= len;
        }
      }

      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        Vertex vert{};

        vert.position[0] = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
        vert.position[1] = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
        vert.position[2] = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

        // Check if `normal_index` is zero or positive. negative = no normal
        // data
        if (idx.normal_index >= 0) {
          vert.normal[0] = attrib.normals[3 * size_t(idx.normal_index) + 0];
          vert.normal[1] = attrib.normals[3 * size_t(idx.normal_index) + 1];
          vert.normal[2] = attrib.normals[3 * size_t(idx.normal_index) + 2];
        } else {
          vert.normal[0] = faceNormal[0];
          vert.normal[1] = faceNormal[1];
          vert.normal[2] = faceNormal[2];
        }

        // Check if `texcoord_index` is zero or positive. negative = no texcoord
        // data
        if (idx.texcoord_index >= 0) {
          vert.texcoord[0] =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
          vert.texcoord[1] =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
        }

        // Optional: vertex colors
        // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
        // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
        // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

        vertices.push_back(vert);
        indices.push_back(static_cast<uint>(indices.size()));
      }
      index_offset += fv;

      // per-face material
      // (void)shapes[s].mesh.material_ids[f];
    }
  }

  name = obj.substr(2);
  /* end of obj loading */
}

void Model::SetMaterial(InstanceData* instance, const tinyobj::material_t &material)
{
  instance->m_amb = vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
  instance->m_dif = vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
  instance->m_spec = vec3(material.specular[0], material.specular[1], material.specular[2]);
  instance->m_shine = material.shininess;

  if (iVBO != 0 && !instance_data.empty()) {
    instances = instance_data.size();
    glBindBuffer(GL_ARRAY_BUFFER, iVBO);
    glBufferData(GL_ARRAY_BUFFER, instances * sizeof(InstanceData),
                 instance_data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void Model::Upload() {
  /* beginning of gpu upload */
  GenerateBuffers();

  if (instance_data.empty()) {
    instance_data.push_back(MakeDefaultInstance());
  }

  instances = instance_data.size();

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, iVBO);
  glBufferData(GL_ARRAY_BUFFER, instances * sizeof(InstanceData),
               instance_data.data(), GL_STATIC_DRAW);

  /* beginning of instance attribute layout */
  for (size_t i = 0; i < 4; i++) {
    GLint ind = 3 + i;
    glEnableVertexAttribArray(ind);
    glVertexAttribPointer(ind, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                          (void *)(offsetof(InstanceData, m_matrix) +
                                   (i * sizeof(vec4))));
    glVertexAttribDivisor(ind, 1);
  }

  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                        (void *)offsetof(InstanceData, m_amb));
  glVertexAttribDivisor(7, 1);

  glEnableVertexAttribArray(8);
  glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                        (void *)offsetof(InstanceData, m_dif));
  glVertexAttribDivisor(8, 1);

  glEnableVertexAttribArray(9);
  glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                        (void *)offsetof(InstanceData, m_spec));
  glVertexAttribDivisor(9, 1);

  glEnableVertexAttribArray(10);
  glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                        (void *)offsetof(InstanceData, m_shine));
  glVertexAttribDivisor(10, 1);
  /* end of instance attribute layout */

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texcoord));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
  /* end of gpu upload */
}

void Model::GenerateBuffers() {
  /* beginning of buffer generation */
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &iVBO);
  /* end of buffer generation */
}

void Model::SetInstanceTransforms(const vector<mat4> &transforms) {
  if (transforms.empty()) {
    instance_data.assign(1, MakeDefaultInstance());
  } else {
    InstanceData base = instance_data.empty() ? MakeDefaultInstance() : instance_data[0];
    instance_data.resize(transforms.size(), base);
    for (size_t i = 0; i < transforms.size(); ++i) {
      instance_data[i].m_matrix = transforms[i];
    }
  }

  instances = instance_data.size();

  if (iVBO != 0) {
    glBindBuffer(GL_ARRAY_BUFFER, iVBO);
    glBufferData(GL_ARRAY_BUFFER, instances * sizeof(InstanceData),
                 instance_data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void Model::SetInstanceData(const vector<InstanceData> &data) {
  if (data.empty()) {
    instance_data.assign(1, MakeDefaultInstance());
  } else {
    instance_data = data;
  }

  instances = instance_data.size();

  if (iVBO != 0) {
    glBindBuffer(GL_ARRAY_BUFFER, iVBO);
    glBufferData(GL_ARRAY_BUFFER, instances * sizeof(InstanceData),
                 instance_data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void Model::Draw(const mat4 &viewMtx, const mat4 &projMtx, const vec3 &cameraPos) { 
  /* beginning of draw call */
  GLuint program = shader.GetProgram();
  glUseProgram(program);

  GLint locView = glGetUniformLocation(program, "view");
  GLint locProj = glGetUniformLocation(program, "proj");
  GLint locLPos = glGetUniformLocation(program, "l_pos");
  GLint locVPos = glGetUniformLocation(program, "v_pos");
  GLint locLCol = glGetUniformLocation(program, "l_col");
  GLint locKA = glGetUniformLocation(program, "m_amb");
  GLint locKD = glGetUniformLocation(program, "m_dif");
  GLint locKS = glGetUniformLocation(program, "m_spec");
  GLint locNS = glGetUniformLocation(program, "m_shine");
  GLint locUseTex = glGetUniformLocation(program, "use_tex");

  if (locView >= 0)
    glUniformMatrix4fv(locView, 1, GL_FALSE, value_ptr(viewMtx));
  if (locProj >= 0)
    glUniformMatrix4fv(locProj, 1, GL_FALSE, value_ptr(projMtx));
  if (locLPos >= 0)
    glUniform3f(locLPos, 3.0f, 3.0f, 3.0f);
  if (locVPos >= 0)
    glUniform3f(locVPos, cameraPos.x, cameraPos.y, cameraPos.z);
  if (locLCol >= 0)
    glUniform3f(locLCol, 1.0f, 1.0f, 1.0f);

  if (!instance_data.empty()) {
    const InstanceData &first = instance_data[0];
    if (locKA >= 0)
      glUniform3f(locKA, first.m_amb.x, first.m_amb.y, first.m_amb.z);
    if (locKD >= 0)
      glUniform3f(locKD, first.m_dif.x, first.m_dif.y, first.m_dif.z);
    if (locKS >= 0)
      glUniform3f(locKS, first.m_spec.x, first.m_spec.y, first.m_spec.z);
    if (locNS >= 0)
      glUniform1f(locNS, first.m_shine > 0.0f ? first.m_shine : 32.0f);
  }

  if (locUseTex >= 0)
    glUniform1i(locUseTex, 0);

  glBindVertexArray(VAO);
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                          GL_UNSIGNED_INT, 0, instances);
  glBindVertexArray(0);

  glUseProgram(0);
  /* end of draw call */
}

void Model::DestroyBuffers() {
  if (iVBO) {
    glDeleteBuffers(1, &iVBO);
  }

  if (VBO) {
    glDeleteBuffers(1, &VBO);
  }

  if (EBO) {
    glDeleteBuffers(1, &EBO);
  }

  if (VAO) {
    glDeleteVertexArrays(1, &VAO);
  }
}

void Model::LoadShader(string path, string name) {
  string frag_name = path + name + ".frag";
  string vert_name = path + name + ".vert";

  if (!shader.LoadVertexShader(vert_name.c_str())) {
    fprintf(stderr, "Failed to compile vertex shader %s\n", vert_name.c_str());
    return;
  }

  if (!shader.LoadFragmentShader(frag_name.c_str())) {
    fprintf(stderr, "Failed to compile fragment shader %s\n",
            frag_name.c_str());
    return;
  }

  if (!shader.Create()) {
    fprintf(stderr, "Failed to link shader program for %s\n", name.c_str());
    return;
  }

  if (!shader.GetProgram()) {
    fprintf(stderr, "Failed to create shader program for %s", name.c_str());
    return;
  }
}

void Model::Tr(vec3 offset)
{
  /* beginning of first-instance translation */
  if (instance_data.empty()) {
    instance_data.push_back(MakeDefaultInstance());
  }

  instance_data[0].m_matrix = translate(instance_data[0].m_matrix, offset);
  /* end of first-instance translation */
}

void Model::Sc(vec3 offset)
{
  /* beginning of first-instance scaling */
  if (instance_data.empty()) {
    instance_data.push_back(MakeDefaultInstance());
  }

  instance_data[0].m_matrix = scale(instance_data[0].m_matrix, offset);
  /* end of first-instance scaling */
}