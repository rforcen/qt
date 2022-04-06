//
// gl pointer version

#pragma once

#include "renderer.h"
#include "mesh.h"

class glpnt_widget : public Renderer {
  Q_OBJECT

protected:
  void draw() override;
  void init() override;

private:
public:
  glpnt_widget(QWidget *parent = nullptr);
  void set_poly(Polyhedron *poly);

  Mesh mesh;
};

