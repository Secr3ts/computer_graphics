#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define W_WIDTH 960
#define W_HEIGHT 540

void Render() {
  glEnable(GL_CULL_FACE); 
  
  glClearColor(0.5f, 0.5f, 0.5f, 1.f); 
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  

  // glRotatef(0.71f, 0.f, 0.f, 1.f);
  
  glTranslatef(0.2f, 0.f, 0.f);
  //glLoadIdentity();
  glRotatef(-15.f, 0.f, 0.f, 1.f);
  glScalef(1.5f, 1.5f, 1.5f);

  glBegin(GL_TRIANGLES);
  
  glColor3f(1.f, 0.f, 0.f); 
  glVertex2f(0.0f, 0.5f);

  
  glColor3f(0.f, 0.f, 1.f); 
  glVertex2f(-0.5f, -0.5f);

  glColor3f(0.f, 1.f, 0.f); 
  glVertex2f(0.5f, -0.5f);

  glEnd();
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