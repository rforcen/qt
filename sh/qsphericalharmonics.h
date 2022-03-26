#ifndef QSPHERICALHARMONICS_H
#define QSPHERICALHARMONICS_H

#include <QMouseEvent>
#include <QOpenGLWidget>

#include "SH.h"
#include "SphericalHarmonics.h"

class QSphericalHarmonics : public QOpenGLWidget {
  Q_OBJECT

public:
  explicit QSphericalHarmonics(QWidget *parent = nullptr);
  ~QSphericalHarmonics() { delete sh; }

private:
  float zoom = -6;
  QPointF last, angle;

  bool moving = false;

  int code = 9, colormap = 7, resol = 128 * 2;
  SphericalHarmonics *sh = new SphericalHarmonics(code, resol, colormap);
  SH *shm = new SH(code, resol, colormap);

private:
  void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear,
                     GLdouble zFar) {
    GLdouble fH = tan(fovY / 360. * M_PI) * zNear, fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
  }
  void sceneInit() { // works nice for golden solid colors (requires normals)
    float lmodel_ambient[] = {0, 0, 0, 0};
    float lmodel_twoside[] = {GL_FALSE};
    float light0_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float light0_diffuse[] = {1.0f, 1.0f, 1.0f, 0.0f};
    float light0_position[] = {1, 0.5, 1, 0};
    float light1_position[] = {-1, 0.5, -1, 0};
    float light0_specular[] = {1, 1, 1, 0};
    float bevel_mat_ambient[] = {0, 0, 0, 1};
    float bevel_mat_shininess[] = {40};
    float bevel_mat_specular[] = {1, 1, 1, 0};
    float bevel_mat_diffuse[] = {1, 0, 0, 0};

    //  glClearColor(float(color.redF()), float(color.greenF()),
    //  float(color.blueF()),               1);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glEnable(GL_LIGHT1);

    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glEnable(GL_LIGHTING);

    glMaterialfv(GL_FRONT, GL_AMBIENT, bevel_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, bevel_mat_shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bevel_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bevel_mat_diffuse);

    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LINE_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glClearDepth(1.0);       // Set background depth to farthest
    glEnable(GL_DEPTH_TEST); // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);  // Set the type of depth-test
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,
           GL_NICEST); //  Nice perspective corrections
  }

  void savePLY() {
    if (sh)
      sh->savePLY();
  }
  void saveCTM() {
    if (sh)
      sh->saveCTM();
  }
  string getFileName(string ext) { return sh->fileName(ext); }

protected:
  void initializeGL() override { sceneInit(); }
  void paintGL() override;
  void resizeGL(int width, int height) override;

  void mousePressEvent(QMouseEvent *) override { moving = true; }
  void mouseReleaseEvent(QMouseEvent *) override { moving = false; }
  void mouseMoveEvent(QMouseEvent *event) override {
    if (moving) {
      angle += (event->position() - last) / 2;
      last = event->position();

      update();
    }
  }
  void wheelEvent(QWheelEvent *event) override {
    zoom += event->angleDelta().y() / 128.0;
    update();
  }

public slots:
  void setCode(int);
  void setColorMap(int);
  void setResol(int);
  void save();

signals:
};

#endif // QSPHERICALHARMONICS_H
