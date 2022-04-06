#include "renderer.h"

Renderer::Renderer(QWidget *parent) : QOpenGLWidget(parent) { _init(); }
void Renderer::_init() { setFPS(15); }
void Renderer::setFPS(float fps) {
  QTimer *timer = new QTimer(this);
  if (fps != 0) {
    timer->start(fps ? 1000. / fps : 0);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
  } else
    timer->stop();
}
void Renderer::repaint() { update(); }
void Renderer::perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear,
                             GLdouble zFar) {
  GLdouble fH = tan(fovY / 360. * M_PI) * zNear, fW = fH * aspect;
  glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}
void Renderer::initializeGL() { // Set up the rendering context, load shaders
                                // and other resources, etc.:
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  init(); // init gl items here
}
void Renderer::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  perspectiveGL(45.0f, (float)w / h, 1, 1000.f);
  glMatrixMode(GL_MODELVIEW);
}
void Renderer::mousePressEvent(QMouseEvent *event) {
  m_lastPos = event->pos();
  emit clicked(event);
}
void Renderer::mouseDoubleClickEvent(QMouseEvent *event) {
  emit doubleClicked(event);
}

void Renderer::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->x() - m_lastPos.x();
  int dy = event->y() - m_lastPos.y();

  if (event->buttons() & Qt::LeftButton) {
    setXRotation(m_xRot + 8 * dy);
    setYRotation(m_yRot + 8 * dx);
  } else if (event->buttons() & Qt::RightButton) {
    setXRotation(m_xRot + 8 * dy);
    setZRotation(m_zRot + 8 * dx);
  }
  m_lastPos = event->pos();
}

void Renderer::wheelEvent(QWheelEvent *event) {
  int wd = event->angleDelta().y() / 120;
  zoom += wd;
}

static void qNormalizeAngle(int &angle) {
  while (angle < 0)
    angle += 360 * 16;
  while (angle > 360 * 16)
    angle -= 360 * 16;
}
void Renderer::setXRotation(int angle) {
  qNormalizeAngle(angle);
  if (angle != m_xRot) {
    m_xRot = angle;
    emit xRotationChanged(angle);
    update();
  }
}
void Renderer::setYRotation(int angle) {
  qNormalizeAngle(angle);
  if (angle != m_yRot) {
    m_yRot = angle;
    emit yRotationChanged(angle);
    update();
  }
}
void Renderer::setZRotation(int angle) {
  qNormalizeAngle(angle);
  if (angle != m_zRot) {
    m_zRot = angle;
    emit zRotationChanged(angle);
    update();
  }
}

void Renderer::paintGL() { // Draw the scene:
  if (doRotate) {
    glClearColor(0, 0, 0, 1);
    glLoadIdentity();
    glTranslatef(0, 0, zoom);

    glRotatef(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    glRotatef(m_yRot / 16.0f, 0, 1, 0);
    glRotatef(m_zRot / 16.0f, 0, 0, 1);
    glRotatef(rotAngle, 1, 1, 1);
  }
  draw();
}
