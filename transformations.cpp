#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <stdio.h>

#define W_WIDTH 960
#define W_HEIGHT 540

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

void Render() {
  glEnable(GL_CULL_FACE);

  float aspect = (float)W_WIDTH / (float)W_HEIGHT;
  float size = 1.0f;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // gluOrtho2D(0.f, W_WIDTH, 0.f, W_HEIGHT);
  gluPerspective(60.0, aspect, 0.1f, 1000.f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Place la caméra au-dessus de la scène (vue de dessus)
  gluLookAt(
      0.0, 5.0, 0.0, // eyeX, eyeY, eyeZ : caméra au-dessus sur l'axe Y
      0.0, 0.0, 0.0, // centerX, centerY, centerZ : regarder l'origine
      0.0, 0.0,
      1.0 // upX, upY, upZ : vecteur 'up' = +Z (orienté vers le haut du monde)
  );

  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  // traduire en pixels
  float tx = 200.f, ty = 150.f, tz = -glutGet(GLUT_ELAPSED_TIME) / 500.f;
  float Tr[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, tx, ty, tz, 1};
  glMultMatrixf(Tr);

  // rotation : convertir en radians
  float time_elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  float angleDeg = time_elapsed * 25.f;
  float a = angleDeg * (3.14159265f / 180.f);
  float Rz[16] = {cosf(a), sinf(a), 0, 0, -sinf(a), cosf(a), 0, 0,
                  0,       0,       1, 0, 0,        0,       0, 1};
  glMultMatrixf(Rz);

  // échelle en pixels
  float s = 200.f;
  float S[16] = {s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  glMultMatrixf(S);

  glClearColor(0.5f, 0.5f, 0.5f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  Triangle();
  glPopMatrix();
}

void Display() {
  Render();
  glutSwapBuffers();
}

int main(int argc, char **argv) {
  printf("START PRG\n");
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutInitWindowSize(W_WIDTH, W_HEIGHT);
  glutCreateWindow("Triangle");
  glutDisplayFunc(Display);
  // IF WE WANT CONTINUOUS REDRAWS:
  glutIdleFunc(Display);

  glutMainLoop();

  return EXIT_SUCCESS;
}