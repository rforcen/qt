#pragma once

#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QTimer>

#include <math.h>
#include <opengl.h>
#include <random>

class Renderer : public QOpenGLWidget, public OpenGL {
  Q_OBJECT

public:
  explicit Renderer(QWidget *parent = 0);
  float rotAngle = 0;
  void setZoom(float zoom) { this->zoom = zoom; }
  void rotate(float ang) { rotAngle += ang; }
  void setFPS(float fps);
  void setRotate(bool doRotate = false) { this->doRotate = doRotate; }
  bool doRotate = true;

private slots:
  void repaint();
public slots:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);
  void clicked(QMouseEvent *event);
  void doubleClicked(QMouseEvent *event);

private:
  void _init();
  void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear,
                     GLdouble zFar);
  float angle = 0, zoom = -4;
  QPoint m_lastPos;
  int m_xRot = 0, m_yRot = 0, m_zRot = 0;

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

  virtual void draw() = 0;
  virtual void init() = 0;
};
