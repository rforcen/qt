#include "qsphericalharmonics.h"
#include "common.h"

QSphericalHarmonics::QSphericalHarmonics(QWidget *parent)
    : QOpenGLWidget(parent) {}

void QSphericalHarmonics::resizeGL(int w, int h) {

  glViewport(0, 0, w * 2, h * 2);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  perspectiveGL(45, (float)w / h, 0.1, 100);
  glMatrixMode(GL_MODELVIEW);
}
void QSphericalHarmonics::paintGL() {

  glClearColor(0.6, 0.6, 0.7, 0.6);
  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

  glLoadIdentity(); // Reset the model-view matrix

  glTranslatef(0, 0, zoom); // zoom

  glRotatef(-angle.x(), 0, 1, 0);
  glRotatef(angle.y(), 1, 0, 0);

  sh->draw();
}

// slots
void QSphericalHarmonics::setCode(int code) {
  this->code = code;
  sh->ReadCode(code);

  update();
}

void QSphericalHarmonics::setColorMap(int cm) {
  this->colormap = cm;
  sh->setColor(cm);
  update();
}

void QSphericalHarmonics::setResol(int resol) {
  this->resol = resol;
  sh->setResolution(resol);
  update();
}

void QSphericalHarmonics::save() { saveCTM(); }
