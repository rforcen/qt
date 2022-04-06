#pragma once
#include <math.h>

#include <QOpenGLWidget>
#include <random>

class OpenGL {
  void push() {
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
  }
  void pop() {
    glPopMatrix();
    glPopAttrib();
  }

  static float constexpr pi = float(M_PI);
  static float constexpr pi2 = float(M_PI * 2);

public:
  OpenGL();
  static void setup();

  static void tetrahedron(GLenum mode = GL_LINE_LOOP, QColor *cols = nullptr);
  static void cube(GLenum mode = GL_LINE_LOOP, QColor *cols = nullptr);
  static void octahedron(GLenum mode = GL_LINE_LOOP, QColor *cols = nullptr);
  static void dodecahedron(GLenum mode = GL_LINE_LOOP, QColor *cols = nullptr);
  static void icosahedron(GLenum mode = GL_LINE_LOOP, QColor *cols = nullptr);
  static void sphere(float rad = 1, int sec = 20, QColor *cols = nullptr);

  static float *cubeCoords(int i, float scale);
  static void cubeFace(int iface);
  static void drawScene(void); // draw the complete floor diaedra
  static void cubeTextured();
  static void dodecahedronFace(int nFace);
  static void dodecahedronSolid();

  static float *normalise(float x, float y, float z);
  static float *calcNormal(float *p, float *p1, float *p2);
  static void sphereTextured(float rad, int sec);
  static void sphereLine(float rad = 1, int sec = 20);
  static void chrysanthemum(void);
  static float *textPolygon(int n);
  static float *polygon(int n, float scale, float z);
  static float *grid(int m, int n, float mult);
  static void shuffle(int *array, int n);
  static float angleVect(float *v1, float *v2);
  static void sceneInit(QColor color = Qt::black);
  static void circle(float *center, int nSec, float rad);
  static void panel(float h, float w);
  static void panelFace(float h, float w);

  static float *ellipseCoords(float cx = 0, float cy = 0, float rx = 1,
                              float ry = 1, int num_segments = 100);
  static void ellipse(float cx = 0, float cy = 0, float rx = 1, float ry = 1,
                      int num_segments = 100);

  static void scale(float sc) { glScalef(sc, sc, sc); }
  static void setColor(QColor color) { setColor(color, 1); }
  static void setColor(QColor color, float alpha) {
    glColor4f(float(color.redF()), float(color.greenF()), float(color.blueF()),
              alpha);
  }
  static void enableColorArray() { glEnableClientState(GL_COLOR_ARRAY); }
  static void disableColorArray() { glDisableClientState(GL_COLOR_ARRAY); }
  static void enableColorArrayMaterial() {
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_COLOR_MATERIAL);
  }
  static void disableColorArrayMaterial() {
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
  }
  static void enableNormalArray() { glEnableClientState(GL_NORMAL_ARRAY); }
  static void disableNormalArray() { glDisableClientState(GL_NORMAL_ARRAY); }
  static void setScale(float scale) { glScalef(scale, scale, scale); }
  static void enableTransparency() { glEnable(GL_BLEND); }
  static void disableTexture() { glDisable(GL_TEXTURE_2D); }
  static void enableTexture() { glEnable(GL_TEXTURE_2D); }
  static void clear(QColor color) {
    glClearColor(float(color.redF()), float(color.greenF()),
                 float(color.blueF()), 1);
  }
  static void lightOff() {
    glDisable(GL_LIGHTING);
    disableColorArrayMaterial();
    disableNormalArray();
    setColor(Qt::black);
  }
  static void lightOn() {
    glEnable(GL_LIGHTING);
    setColor(Qt::white);
    enableNormalArray();
    enableColorArrayMaterial();
  }

  // required inits

  //    glEnableClientState(GL_VERTEX_ARRAY);
  //    glEnableClientState(GL_COLOR_ARRAY);
  //    glDisable(GL_DITHER);

  //    glEnable(GL_CULL_FACE);  glCullFace(GL_BACK);  glClearDepthf(1);
  //    glEnable(GL_MULTISAMPLE);
  //    glEnable (GL_LINE_SMOOTH);
  //    glEnable (GL_BLEND);
};
