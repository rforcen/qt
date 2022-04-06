#include "opengl.h"

OpenGL::OpenGL() {}

void OpenGL::tetrahedron(GLenum mode, QColor *cols) {
  static GLuint face[4][3] = {{0, 1, 2}, {0, 2, 3}, {0, 3, 1}, {1, 3, 2}};
  static GLfloat xyz[4][3] = {
      {0, 0, 1.732051f},
      {1.632993f, 0, -0.5773503f},
      {-0.8164966f, 1.414214f, -0.5773503f},
      {-0.8164966f, -1.414214f, -0.5773503f}
      // {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}
  };

  for (int i = 0; i < 4; i++) {
    glBegin(mode);  // GL_TRIANGLE_FAN);
    if (cols != nullptr) {
      QColor tc = cols[i];
      glColor3f(float(tc.redF()), float(tc.greenF()), float(tc.blueF()));
    }
    glVertex3fv(xyz[face[i][0]]);
    glVertex3fv(xyz[face[i][1]]);
    glVertex3fv(xyz[face[i][2]]);
    glEnd();
  }
}

void OpenGL::octahedron(GLenum mode, QColor *cols) {
  static GLuint face[8][3] = {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 1},
                              {1, 4, 5}, {1, 5, 2}, {2, 5, 3}, {3, 5, 4}};
  static GLfloat xyz[6][3] = {{0, 0, 1.414f},  {1.414f, 0, 0},
                              {0, 1.414f, 0},  {-1.414f, 0, 0},
                              {0, -1.414f, 0}, {0, 0, -1.414f}};
  for (int i = 0; i < 8; i++) {
    glBegin(mode);  // GL_TRIANGLE_FAN);
    if (cols != nullptr) {
      QColor tc = cols[i];
      glColor3f(float(tc.redF()), float(tc.greenF()), float(tc.blueF()));
    }

    glVertex3fv(xyz[face[i][0]]);
    glVertex3fv(xyz[face[i][1]]);
    glVertex3fv(xyz[face[i][2]]);
    glEnd();
  }
}

float *OpenGL::cubeCoords(int i, float scale) {
  static GLfloat sz = 0.707f;
  static GLfloat xyz[8][3] = {{sz, sz, sz},   {-sz, sz, sz},  {-sz, -sz, sz},
                              {sz, -sz, sz},  {sz, -sz, -sz}, {sz, sz, -sz},
                              {-sz, sz, -sz}, {-sz, -sz, -sz}};
  static float rv[3];

  for (int j = 0; j < 3; j++) rv[j] = xyz[i][j] * scale;
  return rv;
}

// void OpenGL::cubeWire() {
//	for (int i = 0; i < 6; i++) {
//		glBegin(GL_LINE_LOOP);
//		for (int j = 0; j < 4; j++)
//			glVertex3fv(xyz[face[i][j]]);
//		glEnd();
//	}
//}

void OpenGL::cube(GLenum mode, QColor *cols) {
  static GLfloat sz = 1;
  static GLfloat xyz[8][3] = {{sz, sz, sz},   {-sz, sz, sz},  {-sz, -sz, sz},
                              {sz, -sz, sz},  {sz, -sz, -sz}, {sz, sz, -sz},
                              {-sz, sz, -sz}, {-sz, -sz, -sz}};

  static GLuint face[6][4] = {{3, 0, 1, 2}, {3, 4, 5, 0}, {0, 5, 6, 1},
                              {1, 6, 7, 2}, {2, 7, 4, 3}, {5, 4, 7, 6}};

  for (int i = 0; i < 6; i++) {
    glBegin(mode);  // GL_TRIANGLE_FAN);
    if (cols) {
      QColor tc = cols[i];
      glColor3f(float(tc.redF()), float(tc.greenF()), float(tc.blueF()));
    }

    for (int j = 0; j < 4; j++) glVertex3fv(xyz[face[i][j]]);
    glEnd();
  }
}

void OpenGL::cubeFace(int iface) {  // draw cube face 'iface' 0..5 textured
  static GLuint face[6][4] = {{0, 1, 2, 3},     {4, 5, 6, 7},
                              {8, 9, 10, 11},   {12, 13, 14, 15},
                              {16, 17, 18, 19}, {20, 21, 22, 23}};
  static GLfloat sz = 1;
  static GLfloat xyz[24][3] = {
      {-sz, -sz, sz},  {sz, -sz, sz},  {sz, sz, sz},  {-sz, sz, sz},   // FRONT
      {-sz, -sz, -sz}, {-sz, sz, -sz}, {sz, sz, -sz}, {sz, -sz, -sz},  // BACK
      {-sz, -sz, -sz}, {-sz, -sz, sz}, {-sz, sz, sz}, {-sz, sz, -sz},  // LEFT
      {sz, -sz, -sz},  {sz, sz, -sz},  {sz, sz, sz},  {sz, -sz, sz},   // RIGHT
      {-sz, sz, -sz},  {-sz, sz, sz},  {sz, sz, sz},  {sz, sz, -sz},   // TOP
      {-sz, -sz, -sz}, {sz, -sz, -sz}, {sz, -sz, sz}, {-sz, -sz, sz}   // BOTTOM
  };
  static GLfloat texture[24][2] = {
      {0, 0}, {1, 0}, {1, 1}, {0, 1},   // FRONT side
      {1, 0}, {1, 1}, {0, 1}, {0, 0},   // BACK
      {0, 0}, {1, 0}, {1, 1}, {0, 1},   // LEFT
      {1, 0}, {1, 1}, {0, 1}, {0, 0},   // RIGHT
      {0, 1}, {0, 0}, {1, 0}, {1, 1},   // TOP
      {1, 1}, {0, 1}, {0, 0}, {1, 0}};  // BOTTOM
  iface %= 6;
  glBegin(GL_QUADS);
  for (int i = 0; i < 4; i++) {
    auto ix = face[iface][i];
    glTexCoord2fv(texture[ix]);
    glVertex3fv(xyz[ix]);
  }
  glEnd();
}

// draw the complete floor diaedra
void OpenGL::drawScene(void) {
  cubeFace(1);
  cubeFace(2);
  cubeFace(5);
}

void OpenGL::cubeTextured() {
  for (int i = 0; i < 6; i++) cubeFace(i);
}

void OpenGL::dodecahedron(GLenum mode, QColor *cols) {
  static GLuint face[12][5] = {
      {0, 1, 4, 7, 2},      {0, 2, 6, 9, 3},      {0, 3, 8, 5, 1},
      {1, 5, 11, 10, 4},    {2, 7, 13, 12, 6},    {3, 9, 15, 14, 8},
      {4, 10, 16, 13, 7},   {5, 8, 14, 17, 11},   {6, 12, 18, 15, 9},
      {10, 11, 17, 19, 16}, {12, 13, 16, 19, 18}, {14, 15, 18, 19, 17}};
  static GLfloat xyz[20][3] = {{0, 0, 1.07047f},
                               {0.713644f, 0, 0.797878f},
                               {-0.356822f, 0.618f, 0.797878f},
                               {-0.356822f, -0.618f, 0.797878f},
                               {0.797878f, 0.618f, 0.356822f},
                               {0.797878f, -0.618f, 0.356822f},
                               {-0.934172f, 0.381966f, 0.356822f},
                               {0.136294f, 1., 0.356822f},
                               {0.136294f, -1., 0.356822f},
                               {-0.934172f, -0.381966f, 0.356822f},
                               {0.934172f, 0.381966f, -0.356822f},
                               {0.934172f, -0.381966f, -0.356822f},
                               {-0.797878f, 0.618f, -0.356822f},
                               {-0.136294f, 1., -0.356822f},
                               {-0.136294f, -1., -0.356822f},
                               {-0.797878f, -0.618f, -0.356822f},
                               {0.356822f, 0.618f, -0.797878f},
                               {0.356822f, -0.618f, -0.797878f},
                               {-0.713644f, 0, -0.797878f},
                               {0, 0, -1.07047f}};

  for (int i = 0; i < 12; i++) {
    glBegin(mode);  // GL_TRIANGLE_FAN);
    if (cols) {
      QColor tc = cols[i];
      glColor3f(float(tc.redF()), float(tc.greenF()), float(tc.blueF()));
    }

    for (int j = 0; j < 5; j++) glVertex3fv(xyz[face[i][j]]);
    glEnd();
  }
}

void OpenGL::dodecahedronFace(int nFace) {  // 12 faces
#define nDodecaFaces 12
  static GLuint face[nDodecaFaces][5] = {
      {0, 1, 4, 7, 2},      {0, 2, 6, 9, 3},      {0, 3, 8, 5, 1},
      {1, 5, 11, 10, 4},    {2, 7, 13, 12, 6},    {3, 9, 15, 14, 8},
      {4, 10, 16, 13, 7},   {5, 8, 14, 17, 11},   {6, 12, 18, 15, 9},
      {10, 11, 17, 19, 16}, {12, 13, 16, 19, 18}, {14, 15, 18, 19, 17}};
  static GLfloat xyz[20][3] = {{0, 0, 1.07047f},
                               {0.713644f, 0, 0.797878f},
                               {-0.356822f, 0.618f, 0.797878f},
                               {-0.356822f, -0.618f, 0.797878f},
                               {0.797878f, 0.618f, 0.356822f},
                               {0.797878f, -0.618f, 0.356822f},
                               {-0.934172f, 0.381966f, 0.356822f},
                               {0.136294f, 1, 0.356822f},
                               {0.136294f, -1, 0.356822f},
                               {-0.934172f, -0.381966f, 0.356822f},
                               {0.934172f, 0.381966f, -0.356822f},
                               {0.934172f, -0.381966f, -0.356822f},
                               {-0.797878f, 0.618f, -0.356822f},
                               {-0.136294f, 1., -0.356822f},
                               {-0.136294f, -1., -0.356822f},
                               {-0.797878f, -0.618f, -0.356822f},
                               {0.356822f, 0.618f, -0.797878f},
                               {0.356822f, -0.618f, -0.797878f},
                               {-0.713644f, 0, -0.797878f},
                               {0, 0, -1.07047f}};
  static float *text = textPolygon(5);

  glBegin(GL_TRIANGLE_FAN);
  for (int j = 0; j < 5; j++) {
    glTexCoord2fv(text + j * 2);
    glVertex3fv(xyz[face[nFace][j]]);
  }
  glEnd();
}

void OpenGL::dodecahedronSolid() {  // textured
  for (int i = 0; i < 12; i++) dodecahedronFace(i);
}

void OpenGL::icosahedron(GLenum mode, QColor *cols) {
  float X = .525731112119133606f, Z = .850650808352039932f;

  static GLfloat xyz[12][3] = {{-X, 0.0, Z}, {X, 0.0, Z},   {-X, 0.0, -Z},
                               {X, 0.0, -Z}, {0.0, Z, X},   {0.0, Z, -X},
                               {0.0, -Z, X}, {0.0, -Z, -X}, {Z, X, 0.0},
                               {-Z, X, 0.0}, {Z, -X, 0.0},  {-Z, -X, 0.0}};
  static GLuint face[20][3] = {{0, 4, 1},  {0, 9, 4},  {9, 5, 4},  {4, 5, 8},
                               {4, 8, 1},  {8, 10, 1}, {8, 3, 10}, {5, 3, 8},
                               {5, 2, 3},  {2, 7, 3},  {7, 10, 3}, {7, 6, 10},
                               {7, 11, 6}, {11, 0, 6}, {0, 1, 6},  {6, 1, 10},
                               {9, 0, 11}, {9, 11, 2}, {9, 2, 5},  {7, 2, 11}};

  glBegin(mode);  // GL_TRIANGLE_FAN); glBegin(GL_TRIANGLES);
  for (int i = 0; i < 20; i++) {
    if (cols) {
      QColor tc = cols[i];
      glColor3f(float(tc.redF()), float(tc.greenF()), float(tc.blueF()));
    }

    glVertex3fv(xyz[face[i][0]]);
    glVertex3fv(xyz[face[i][1]]);
    glVertex3fv(xyz[face[i][2]]);
  }
  glEnd();
}

void OpenGL::sphere(float rad, int sec, QColor *cols) {
  static GLfloat **xyz;
  static GLuint **face;

  int n = sec + 1, n2 = n * n, nface = sec * sec;  // calc points

  face = new GLuint *[nface];
  xyz = new GLfloat *[n2];  // 'n2' textures (u,v), normal, colors

  int cv = 0;
  float v = 0, u = 0, du = 1.f / sec, dv = 1.f / sec;
  for (int r = 0; r <= sec; r++) {
    v = r * dv;                    // [0,1]
    float theta1 = float(v * pi);  // [0,PI]

    float xn = 0, yn = 1, zn = 0;  // n=(0,1,0)
    float cosRY = cos(theta1), sinRY = sin(theta1),
          // n.rotateZ(theta1)
        xt = -(yn * sinRY), yt = +(yn * cosRY), zt = 0;
    xn = xt;
    yn = yt;
    zn = zt;

    for (int c = 0; c <= sec; c++) {
      u = c * du;                    // [0,1]
      float theta2 = u * pi2;        // [0,2PI]
      float x = xn, y = yn, z = zn;  // xyz=n

      cosRY = cos(theta2);
      sinRY = sin(theta2);  // xyz.rotateY
      xt = (x * cosRY) + (z * sinRY);
      zt = (x * -sinRY) + (z * cosRY);
      x = xt;
      z = zt;

      x *= rad;
      y *= rad;
      z *= rad;  // xyz*=rad

      xyz[cv] = new float[3]{x, y, z};  // new GLfloat[3];
      // texture coords (u,v)
      cv++;
    }
  }
  int cl = sec + 1, cf = 0;  // Add faces
  for (int r = 0; r < sec; r++) {
    int off = r * cl;
    for (int c = 0; c < sec; c++) {
      face[cf] =
          new GLuint[4]{GLuint(off + c), GLuint(off + c + 1),
                        GLuint(off + (c + 1 + cl)), GLuint(off + (c + 0 + cl))};
      cf++;
    }
  }

  for (int i = 0; i < nface; i++) {  // render
    auto f0 = face[i][0], f1 = face[i][1], f2 = face[i][2], f3 = face[i][3];

    float *nrm = calcNormal(xyz[f0], xyz[f1], xyz[f2]);

    glBegin(GL_TRIANGLE_FAN);
    QColor tc = (cols) ? cols[i] : QColor(Qt::blue);
    glColor3f(float(tc.redF()), float(tc.greenF()), float(tc.blueF()));

    glNormal3fv(nrm);
    glVertex3fv(xyz[f0]);
    glNormal3fv(nrm);
    glVertex3fv(xyz[f1]);
    glNormal3fv(nrm);
    glVertex3fv(xyz[f2]);
    glNormal3fv(nrm);
    glVertex3fv(xyz[f3]);
    glEnd();

    delete[] nrm;
  }

  for (int i = 0; i < nface; i++) delete face[i];
  for (int i = 0; i < n2; i++) delete xyz[i];
  delete[] face;
  delete[] xyz;
}

void OpenGL::sphereLine(float rad, int sec) {
  static GLfloat **xyz;
  static GLuint **face;

  int n = sec + 1, n2 = n * n, nface = sec * sec;  // calc points

  face = new GLuint *[nface];
  xyz = new GLfloat *[n2];

  int cv = 0;
  float v = 0, u = 0, du = 1.f / sec, dv = 1.f / sec;
  for (int r = 0; r <= sec; r++) {
    v = r * dv;             // [0,1]
    float theta1 = v * pi;  // [0,PI]

    float xn = 0, yn = 1, zn = 0;  // n=(0,1,0)
    float cosRY = cos(theta1), sinRY = sin(theta1),
          // n.rotateZ(theta1)
        xt = -(yn * sinRY), yt = +(yn * cosRY), zt = 0;
    xn = xt;
    yn = yt;
    zn = zt;

    for (int c = 0; c <= sec; c++) {
      u = c * du;                    // [0,1]
      float theta2 = u * pi2;        // [0,2PI]
      float x = xn, y = yn, z = zn;  // xyz=n

      cosRY = cos(theta2);
      sinRY = sin(theta2);  // xyz.rotateY
      xt = (x * cosRY) + (z * sinRY);
      zt = (x * -sinRY) + (z * cosRY);
      x = xt;
      z = zt;

      x *= rad;
      y *= rad;
      z *= rad;  // xyz*=rad

      xyz[cv] = new GLfloat[3]{x, y, z};
      // texture coords (u,v)
      cv++;
    }
  }
  int cl = sec + 1, cf = 0;  // Add faces
  for (int r = 0; r < sec; r++) {
    int off = r * cl;
    for (int c = 0; c < sec; c++) {
      face[cf] =
          new GLuint[4]{GLuint(off + c), GLuint(off + c + 1),
                        GLuint(off + (c + 1 + cl)), GLuint(off + (c + 0 + cl))};
      cf++;
    }
  }

  for (int i = 0; i < nface; i++) {  // render
    auto f0 = face[i][0], f1 = face[i][1], f2 = face[i][2], f3 = face[i][3];

    glBegin(GL_LINE_LOOP);

    glVertex3fv(xyz[f0]);
    glVertex3fv(xyz[f1]);
    glVertex3fv(xyz[f2]);
    glVertex3fv(xyz[f3]);

    glEnd();
  }

  for (int i = 0; i < nface; i++) delete face[i];
  for (int i = 0; i < n2; i++) delete xyz[i];
  delete[] face;
  delete[] xyz;
}

float *OpenGL::normalise(float x, float y, float z) {  // Normalise a vector
  float length = x * x + y * y + z * z, *rv;
  if (length > 0) {
    length = sqrt(length);
    x /= length;
    y /= length;
    z /= length;
  } else {
    x = y = z = 0;
  }

  rv = new float[3]{x, y, z};
  return rv;
}

float *OpenGL::calcNormal(float *p, float *p1, float *p2) {  // to norm
  float xa = p1[0] - p[0], ya = p1[1] - p[1], za = p1[2] - p[2];
  float xb = p2[0] - p[0], yb = p2[1] - p[1], zb = p2[2] - p[2];
  float nx = ya * zb - za * yb, ny = za * xb - xa * zb, nz = xa * yb - ya * xb;
  return normalise(nx, ny, nz);
}

void OpenGL::sphereTextured(float rad, int sec) {  // textured version

  GLfloat **xyz;
  GLuint **face;
  GLfloat **texture;

  int n = sec + 1, n2 = n * n, nface = sec * sec;  // calc points

  face = new GLuint *[nface];
  xyz = new GLfloat *[n2];  // 'n2' textures (u,v), normal, colors
  texture = new GLfloat *[n2];

  int cv = 0;
  float v = 0, u = 0, du = 1.f / sec, dv = 1.f / sec;
  for (int r = 0; r <= sec; r++) {
    v = r * dv;             // [0,1]
    float theta1 = v * pi;  // [0,PI]

    float xn = 0, yn = 1, zn = 0;  // n=(0,1,0)
    float cosRY = cos(theta1), sinRY = sin(theta1),
          // n.rotateZ(theta1)
        xt = -(yn * sinRY), yt = +(yn * cosRY), zt = 0;
    xn = xt;
    yn = yt;
    zn = zt;

    for (int c = 0; c <= sec; c++) {
      u = c * du;                    // [0,1]
      float theta2 = u * pi2;        // [0,2PI]
      float x = xn, y = yn, z = zn;  // xyz=n

      cosRY = cos(theta2);
      sinRY = sin(theta2);  // xyz.rotateY
      xt = (x * cosRY) + (z * sinRY);
      zt = (x * -sinRY) + (z * cosRY);
      x = xt;
      z = zt;

      x *= rad;
      y *= rad;
      z *= rad;  // xyz*=rad
      // vertex coords
      xyz[cv] = new GLfloat[3]{x, y, z};
      // texture coords (u,v)
      texture[cv] = new GLfloat[2]{u, v};

      cv++;
    }
  }

  int cl = sec + 1, cf = 0;  // Add faces
  for (int r = 0; r < sec; r++) {
    int off = r * cl;
    for (int c = 0; c < sec; c++) {
      face[cf] =
          new GLuint[4]{GLuint(off + c), GLuint(off + c + 1),
                        GLuint(off + (c + 1 + cl)), GLuint(off + (c + 0 + cl))};
      cf++;
    }
  }

  for (int i = 0; i < nface; i++) {  // render
    glBegin(GL_QUADS);
    auto f0 = face[i][0], f1 = face[i][1], f2 = face[i][2], f3 = face[i][3];

    float *nrm = calcNormal(xyz[f0], xyz[f1], xyz[f2]);

    // order : texture, vertex
    glNormal3fv(nrm);
    glTexCoord2fv(texture[f0]);
    glVertex3fv(xyz[f0]);
    glNormal3fv(nrm);
    glTexCoord2fv(texture[f1]);
    glVertex3fv(xyz[f1]);
    glNormal3fv(nrm);
    glTexCoord2fv(texture[f2]);
    glVertex3fv(xyz[f2]);
    glNormal3fv(nrm);
    glTexCoord2fv(texture[f3]);
    glVertex3fv(xyz[f3]);

    delete[] nrm;

    glEnd();
  }

  // delete geo
  for (int i = 0; i < nface; i++) delete face[i];
  for (int i = 0; i < n2; i++) {
    delete xyz[i];
    delete texture[i];
  }
  delete[] face;
  delete[] xyz;
  delete[] texture;
}

void OpenGL::chrysanthemum(void) {
  int N = 3000;
  float u, r, p4, p8;
  float x, y, z;

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= N; i++) {
    u = i * 21.0f * float(M_PI) / N;
    p4 = sinf(17 * u / 3);
    p8 = sinf(2 * cos(3 * u) - 28 * u);
    r = 5 * (1 + sin(11 * u / 5)) -
        4 * p4 * p4 * p4 * p4 * p8 * p8 * p8 * p8 * p8 * p8 * p8 * p8;
    x = r * cos(u);
    y = r * sin(u);
    z = (r / 20 + .2f) * sinf(r * (float(M_PI) * 2) / 7);
    if (i > 0) {
      QColor tc = QColor(Qt::red);
      glColor3f(float(tc.redF()), float(tc.greenF()), float(tc.blueF()));
      glVertex3f(x, y, z);
    }
  }
  glEnd();
}

void OpenGL::setup() {
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);  // non transparent faces
  glFlush();
}

float *OpenGL::textPolygon(
    int n) {  // generate texture polygon coords [u,v] x n
  float *c = new float[n * 2];
  float pi2n = 6.28318530718f / n, r = 0.5f, xoffset = 0.5f, yoffset = xoffset;
  for (int i = 0; i < n; i++) {
    float a = n - i - n / 2.0f;
    c[i * 2 + 0] = r * sin(a * pi2n) + xoffset;
    c[i * 2 + 1] = r * cos(a * pi2n) + yoffset;
  }
  return c;
}

float *OpenGL::polygon(int n, float scale, float z) {
  float pi2n = 6.28318530718f / n;
  float *c = new float[n * 3];
  for (int i = 0; i < n; i++) {
    c[i * 3 + 0] = scale * sinf(i * pi2n);
    c[i * 3 + 1] = scale * cosf(i * pi2n);
    c[i * 3 + 2] = z * scale;
  }
  return c;
}

float *OpenGL::grid(int m, int n, float mult) {
  float *gr = new float[n * m * 2];

  for (int i = 0, ic = 0; i < m; i++) {
    for (int j = 0; j < n; j++, ic += 2) {
      gr[ic + 0] = mult * float(j) / n;
      gr[ic + 1] = mult * float(i) / m;
    }
  }
  return gr;
}

void OpenGL::shuffle(int *array, int n) {
  if (n > 1) {
    for (int i = n - 1; i > 0; i--) {
      int j = rand() % (i + 1), t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}

float OpenGL::angleVect(float *v1, float *v2) {  // res=ang(v1,v2)
  // angle = atan2(norm(cross(a,b)), dot(a,b))
  float cp = 0, m1 = 0, m2 = 0;
  for (int i = 0; i < 3; i++) {  // cross
    cp += (v1[i] * v2[i]);
    m1 += (v1[i] * v1[i]);
    m2 += (v2[i] * v2[i]);
  }
  float xv = cp / (sqrtf(m1) * sqrtf(m2));
  if (fabs(xv) > 1) xv = 0;
  return acosf(xv);
}

void OpenGL::sceneInit(
    QColor color) {  // works nice for golden solid colors (requires normals)
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

  glClearColor(float(color.redF()), float(color.greenF()), float(color.blueF()),
               1);

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
  //  glEnable(GL_POLYGON_SMOOTH); // creates white shade on lines -> remove
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void OpenGL::circle(float *center, int nSec, float rad) {
  float *cirCoords = polygon(nSec, 1, rad);
  glBegin(GL_LINE_LOOP);
  if (center != nullptr) {
    for (int i = 0; i < nSec; i++) {
      glVertex3f(center[0] + cirCoords[i * 3 + 0],
                 center[1] + cirCoords[i * 3 + 1], center[2]);
    }
  } else {
    for (int i = 0; i < nSec; i++)
      glVertex3f(cirCoords[i * 3 + 0], cirCoords[i * 3 + 1], 0);
  }
  glEnd();
  delete[] cirCoords;
}

void OpenGL::panel(float h, float w) {
  float w2 = w / 2, h2 = h / 2;
  float coords[] = {-w2, -h2, -w2, h2, w2, h2, w2, -h2};
  glBegin(GL_POLYGON);
  for (int i = 0; i < 4; i++) glVertex2fv(coords + i * 2);
  glEnd();
}

void OpenGL::panelFace(float h, float w) {
  float w2 = w / 2, h2 = h / 2;
  float coords[] = {-w2, -h2, w2, -h2, w2, h2, -w2, h2};
  float text[] = {0, 0, 1, 0, 1, 1, 0, 1};
  glBegin(GL_QUADS);
  for (int i = 0; i < 4; i++) {
    glTexCoord2fv(text + i * 2);
    glVertex2fv(coords + i * 2);
  }
  glEnd();
}

void OpenGL::ellipse(float cx, float cy, float rx, float ry, int num_segments) {
  float theta = 2 * pi / num_segments;
  float c = cosf(theta);  // precalculate the sine and cosine
  float s = sinf(theta);
  float t;

  float x = 1;  // we start at angle = 0
  float y = 0;

  glBegin(GL_LINE_LOOP);
  for (int ii = 0; ii < num_segments; ii++) {
    // apply radius and offset
    glVertex2f(x * rx + cx, y * ry + cy);  // output vertex

    // apply the rotation matrix
    t = x;
    x = c * x - s * y;
    y = s * t + c * y;
  }
  glEnd();
}

/*
 int nc=100;
 float*ecrd = ellipseCoords(0, 0, 1, 1.2, nc); }

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, ecrd);
  glDrawArrays(GL_LINE_LOOP, 0, nc);
*/
float *OpenGL::ellipseCoords(float cx, float cy, float rx, float ry,
                             int num_segments) {
  float theta = 2 * M_PI / float(num_segments);
  float c = cosf(theta);  // precalculate the sine and cosine
  float s = sinf(theta);
  float t;
  float *coords = new float[num_segments * 2];

  float x = 1;  // we start at angle = 0
  float y = 0;

  for (int ii = 0; ii < num_segments; ii++) {
    coords[ii * 2 + 0] = x * rx + cx;
    coords[ii * 2 + 1] = y * ry + cy;  // output vertex

    // apply the rotation matrix
    t = x;
    x = c * x - s * y;
    y = s * t + c * y;
  }
  return coords;
}
