#include "gl01_widget.h"

gl01_widget::gl01_widget(QWidget *parent) : Renderer(parent) {}

void gl01_widget::init() {
  sceneInit();
  setZoom(-3.5);
}

void gl01_widget::draw() {
  if (poly)
    glCallList(1);
}

void gl01_widget::draw_poly() {
  if (poly) {
    int ixf = 0;
    for (auto &face : poly->faces) {
      glBegin(GL_POLYGON);

      auto normal = poly->get_normal(ixf);
      normal.normalize();
      glNormal3fv((GLfloat *)(&normal));

      auto color = poly->get_color(ixf);
      glColor3fv(reinterpret_cast<GLfloat *>(&color));

      for (auto ix_coord : face) {
        glVertex3fv(
            reinterpret_cast<GLfloat *>(&poly->vertexes[size_t(ix_coord)]));
      }
      glEnd();
      ixf++;
    }
  }
}

void gl01_widget::draw_lines() {
  for (auto &face : poly->faces) {
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);

    for (auto ix_coord : face) {
      auto v = poly->vertexes[size_t(ix_coord)];
      glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
  }
}

void gl01_widget::set_poly(Polyhedron *poly) {
  this->poly = poly;

  glNewList(1, GL_COMPILE);

  draw_poly();
  draw_lines();

  glEndList();

  update();
}
