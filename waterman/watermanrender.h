#pragma once
#include <QObject>
#include <Waterman.h>
#include <renderer.h>

class WatermanRender : public Renderer {
  Q_OBJECT
public:
  WatermanPoly wp;
  int compiledScene = 4;

  WatermanRender(QWidget *parent = nullptr);
  ~WatermanRender() { glDeleteLists(compiledScene, 1); }

  void setRadius(double rad) {
    wp.genHull(rad);

    glNewList(compiledScene, GL_COMPILE);
    render();
    glEndList();

    update();
  }

  int getnFaces() { return wp.hull->getNumFaces(); }
  int getnVertex() { return wp.hull->getNumVertices(); }

  void render() {
    class Local {
    public:
      static void scale(vector<double> &vertex) {
        double max = 0;
        for (auto &v : vertex)
          max = std::max(max, abs(v));
        if (max != 0)
          for (auto &v : vertex)
            v /= max;
      }
    };

    GLubyte colorsb[] = {0,   18,  25,  0,   95,  115, 10,  147, 150, 148,
                         210, 189, 233, 216, 166, 238, 155, 0,   202, 103,
                         2,   187, 62,  3,   174, 32,  18,  155, 34,  38};

    auto vertex = wp.hull->getVertex();
    Local::scale(vertex);

    // faces
    for (auto &faces : wp.hull->getFaces()) {
      glBegin(GL_POLYGON);
      glColor3ubv(colorsb + (faces.size() % 10) * 3);

      for (auto ix : faces)
        glVertex3dv(vertex.data() + (ix * 3));

      glEnd();

      // lines
      glColor3f(0, 0, 0);
      for (auto &faces : wp.hull->getFaces()) {
        glBegin(GL_LINE_LOOP);
        for (auto ix : faces)
          glVertex3d(vertex[ix * 3 + 0], vertex[ix * 3 + 1],
                     vertex[ix * 3 + 2]);

        glEnd();
      }
    }
  }

  void init() override{};
  void draw() override { glCallList(compiledScene); }
};
