#include "modern.h"
#include "GL/glew.h"
#include "GL/gl.h"

#include "common/GLShader.h"

#include <GL/freeglut_std.h>
#include <cstdlib>

#define W_WIDTH 1080
#define W_HEIGHT 720

GLShader g_BasicShader;
GLuint VBO;
static const float triangle[] = {
      -0.5f, -0.5f, 153.f / 255.f, 1.f,          51.f / 255.f,
      0.5f,  -0.5f, 51.f / 255.f,  51.f / 255.f, 1.f,
      0.0f,  0.5f,  102.f / 255.f, 0.f,          204.f / 255.f};

constexpr size_t N = (sizeof(triangle) / sizeof(float)) / 5;


struct Vec3f {
  union {
    struct {
      float x, y, z;
    };
    float v[3];
  };

  Vec3f() : x(0), y(0), z(0) {}
  Vec3f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

  float &operator[](size_t i) { return v[i]; }
  const float &operator[](size_t i) const { return v[i]; }
};

struct Color3f {
  union {
    struct {
      float r, g, b;
    };
    float v[3];
  };

  Color3f() : r(0), g(0), b(0) {}
  Color3f(float R, float G, float B) : r(R), g(G), b(B) {}

  float &operator[](size_t i) { return v[i]; }
  const float &operator[](size_t i) const { return v[i]; }
};

struct Vec2f {
  union {
    struct {
      float x, y;
    };
    float v[2];
  };

  Vec2f() : x(0), y(0) {}
  Vec2f(float X, float Y) : x(X), y(Y) {}

  float &operator[](size_t i) { return v[i]; }
  const float &operator[](size_t i) const { return v[i]; }
};

struct Vertex {
  Vec2f position;
  Color3f color;

  Vertex() : position(), color() {}
  Vertex(Vec2f P, Color3f C) : position(P), color(C) {}
};

void Triangle() {
  glBegin(GL_TRIANGLES);

  glColor3f(1.f, 0.f, 0.f);
  glVertex2f(0.0f, 0.5f);

  glColor3f(0.f, 0.f, 1.f);
  glVertex2f(-0.5f, -0.5f);

  glColor3f(0.f, 1.f, 0.f);
  glVertex2f(0.5f, -0.5f);

  glEnd();
}

bool Initialize() {
  g_BasicShader.LoadVertexShader("basic.vs");
  g_BasicShader.LoadFragmentShader("basic.fs");
  g_BasicShader.Create();

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 2* 3, triangle, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  

#ifdef WIN32
  wglSwapIntervalEXT(1);
#endif
  return true;
}

void Render() {
  glViewport(0, 0, W_WIDTH, W_HEIGHT);

  glClearColor(0.5f, 0.5f, 0.5f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  /**
  auto basicProgram = g_BasicShader.GetProgram();
  glUseProgram(basicProgram);

  int loc_position = glGetAttribLocation(basicProgram, "a_position");
  int loc_color = glGetAttribLocation(basicProgram, "a_color");

  Vertex v[N];
  for (size_t i = 0, vi = 0; vi < N; ++vi, i += 5) {
    v[vi] = Vertex(Vec2f(triangle[i], triangle[i + 1]),
                   Color3f(triangle[i + 2], triangle[i + 3], triangle[i + 4]));
  }

  glEnableVertexAttribArray(loc_position);
  glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        &v[0].position);

  glEnableVertexAttribArray(loc_color);
  glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        &v[0].color);
  */

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
  glEnableVertexAttribArray(0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glutSwapBuffers();
}

void Terminate() {
  g_BasicShader.Destroy();
  glDeleteBuffers(1, &VBO);
}

int main() { return EXIT_SUCCESS; }