#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define W_HEIGHT 540
#define W_WIDTH 960

void viewport(float x, float y) // 2.1
{
  glViewport(x, y, W_WIDTH / 2, W_WIDTH / 2);
  glScissor(x, y, W_WIDTH, W_HEIGHT);
  glBegin(GL_TRIANGLES);

  // Ex 1.1 Different colors per corner
  glColor3f(1.f, 0.f, 0.f); // red
  glVertex2f(0.0f, 0.5f);

  // 1.2 Invert 2 & 3
  glColor3f(0.f, 0.f, 1.f); // blue
  glVertex2f(-0.5f, -0.5f);

  glColor3f(0.f, 1.f, 0.f); // green
  glVertex2f(0.5f, -0.5f);

  glEnd();
}

void Render() {
  glEnable(GL_CULL_FACE); // 1.2
  glEnable(GL_VIEWPORT);  // 2.1
  glEnable(GL_SCISSOR_TEST); // 2.1

  glClearColor(0.5f, 0.5f, 0.5f, 1.f); // grey background
  glClear(GL_COLOR_BUFFER_BIT);

  viewport(0.f, 0.f);
  viewport(0.f, float(W_HEIGHT) / 2);
  viewport(float(W_WIDTH) / 2, 0.f);
  viewport(float(W_WIDTH) / 2, float(W_HEIGHT) / 2);
}

void Display() {
  Render();
  glutSwapBuffers();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutInitWindowSize(W_WIDTH, W_HEIGHT);
  glutCreateWindow("Triangle");
  glutDisplayFunc(Display);
  glutMainLoop();

  return EXIT_SUCCESS;
}