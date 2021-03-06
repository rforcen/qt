// SphericalHarmonics module
#pragma once

#include "Thread.h"
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

using std::vector;

// SH

class SH {
  // vec3
  class vec3 {
  public:
    vec3(){};
    vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    double x = 0, y = 0, z = 0;
    vec3 operator-(vec3 &o) { return vec3{x - o.x, y - o.y, z - o.z}; }
    vec3 operator/(double f) { return vec3{x / f, y / f, z / f}; }

    static vec3 cross(vec3 a, vec3 b) {
      return vec3{(a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z),
                  (a.x * b.y) - (a.y * b.x)};
    }

    float mag() { return sqrtf(x * x + y * y + z * z); }
    vec3 normalize() {
      if (mag() == 0)
        return *this;
      else
        return *this / mag();
    }
  };

public:
  int resolution;
  int color_map;
  int n_code;
  double code[8];

  bool listCreated = false;

  struct Vertex {
    vec3 position, normal, color, texture;
  };

  vector<Vertex> mesh;

  vec3 calcColor(double v, double vmin, double vmax, int type) {
    float dv, vmid;
    vec3 c = {1, 1, 1};
    vec3 c1, c2, c3;
    float ratio;
    if (vmax < vmin) {
      dv = vmin;
      vmin = vmax;
      vmax = dv;
    }
    if (vmax - vmin < 1.0e-6f) {
      vmin -= 1;
      vmax += 1;
    }
    if (v < vmin)
      v = vmin;
    if (v > vmax)
      v = vmax;
    dv = vmax - vmin;
    switch (type) {
    case 0:
    case 1:
      if (v < (vmin + 0.25f * dv)) {
        c.x = 0;
        c.y = 4 * (v - vmin) / dv;
        c.z = 1;
      } else if (v < (vmin + 0.5f * dv)) {
        c.x = 0;
        c.y = 1;
        c.z = 1 + 4 * (vmin + 0.25f * dv - v) / dv;
      } else if (v < (vmin + 0.75f * dv)) {
        c.x = 4 * (v - vmin - 0.5f * dv) / dv;
        c.y = 1;
        c.z = 0;
      } else {
        c.x = 1;
        c.y = 1 + 4 * (vmin + 0.75f * dv - v) / dv;
        c.z = 0;
      }
      break;

    case 2:
      c.x = (v - vmin) / dv;
      c.y = 0;
      c.z = (vmax - v) / dv;
      break;
    case 3:
      c.x = (v - vmin) / dv;
      c.z = c.x;
      c.y = c.x;
      break;
    case 4:
      if (v < (vmin + dv / 6)) {
        c.x = 1;
        c.y = 6 * (v - vmin) / dv;
        c.z = 0;
      } else if (v < (vmin + 2 * dv / 6)) {
        c.x = 1 + 6 * (vmin + dv / 6 - v) / dv;
        c.y = 1;
        c.z = 0;
      } else if (v < (vmin + 3 * dv / 6)) {
        c.x = 0;
        c.y = 1;
        c.z = 6 * (v - vmin - 2 * dv / 6) / dv;
      } else if (v < (vmin + 4 * dv / 6)) {
        c.x = 0;
        c.y = 1 + 6 * (vmin + 3 * dv / 6 - v) / dv;
        c.z = 1;
      } else if (v < (vmin + 5 * dv / 6)) {
        c.x = 6 * (v - vmin - 4 * dv / 6) / dv;
        c.y = 0;
        c.z = 1;
      } else {
        c.x = 1;
        c.y = 0;
        c.z = 1 + 6 * (vmin + 5 * dv / 6 - v) / dv;
      }
      break;

    case 5:
      c.x = (v - vmin) / (vmax - vmin);
      c.y = 1;
      c.z = 0;
      break;
    case 6:
      c.x = (v - vmin) / (vmax - vmin);
      c.y = (vmax - v) / (vmax - vmin);
      c.z = c.x;
      break;
    case 7:
      if (v < (vmin + 0.25f * dv)) {
        c.x = 0;
        c.y = 4 * (v - vmin) / dv;
        c.z = 1 - c.y;
      } else if (v < (vmin + 0.5f * dv)) {
        c.x = 4 * (v - vmin - 0.25f * dv) / dv;
        c.y = 1 - c.x;
        c.z = 0;
      } else if (v < (vmin + 0.75f * dv)) {
        c.y = 4 * (v - vmin - 0.5f * dv) / dv;
        c.x = 1 - c.y;
        c.z = 0;
      } else {
        c.x = 0;
        c.z = 4 * (v - vmin - 0.75f * dv) / dv;
        c.y = 1 - c.z;
      }
      break;
    case 8:
      if (v < (vmin + 0.5f * dv)) {
        c.x = 2 * (v - vmin) / dv;
        c.y = c.x;
        c.z = c.x;
      } else {
        c.x = 1 - 2 * (v - vmin - 0.5f * dv) / dv;
        c.y = c.x;
        c.z = c.x;
      }
      break;
    case 9:
      if (v < (vmin + dv / 3)) {
        c.z = 3 * (v - vmin) / dv;
        c.y = 0;
        c.x = 1 - c.z;
      } else if (v < (vmin + 2 * dv / 3)) {
        c.x = 0;
        c.y = 3 * (v - vmin - dv / 3) / dv;
        c.z = 1;
      } else {
        c.x = 3 * (v - vmin - 2 * dv / 3) / dv;
        c.y = 1 - c.x;
        c.z = 1;
      }
      break;

    case 10:
      if (v < (vmin + 0.2f * dv)) {
        c.x = 0;
        c.y = 5 * (v - vmin) / dv;
        c.z = 1;
      } else if (v < (vmin + 0.4f * dv)) {
        c.x = 0;
        c.y = 1;
        c.z = 1 + 5 * (vmin + 0.2f * dv - v) / dv;
      } else if (v < (vmin + 0.6f * dv)) {
        c.x = 5 * (v - vmin - 0.4f * dv) / dv;
        c.y = 1;
        c.z = 0;
      } else if (v < (vmin + 0.8f * dv)) {
        c.x = 1;
        c.y = 1 - 5 * (v - vmin - 0.6f * dv) / dv;
        c.z = 0;
      } else {
        c.x = 1;
        c.y = 5 * (v - vmin - 0.8f * dv) / dv;
        c.z = 5 * (v - vmin - 0.8f * dv) / dv;
      }
      break;
    case 11:
      c1.x = 200 / 255;
      c1.y = 60 / 255;
      c1.z = 0 / 255;
      c2.x = 250 / 255;
      c2.y = 160 / 255;
      c2.z = 110 / 255;
      c.x = (c2.x - c1.x) * (v - vmin) / dv + c1.x;
      c.y = (c2.y - c1.y) * (v - vmin) / dv + c1.y;
      c.z = (c2.z - c1.z) * (v - vmin) / dv + c1.z;
      break;
    case 12:
      c1.x = 55 / 255;
      c1.y = 55 / 255;
      c1.z = 45 / 255;
      c2.x = 235 / 255;
      c2.y = 90 / 255;
      c2.z = 30 / 255;
      c3.x = 250 / 255;
      c3.y = 160 / 255;
      c3.z = 110 / 255;
      ratio = 0.4f;
      vmid = vmin + ratio * dv;
      if (v < vmid) {
        c.x = (c2.x - c1.x) * (v - vmin) / (ratio * dv) + c1.x;
        c.y = (c2.y - c1.y) * (v - vmin) / (ratio * dv) + c1.y;
        c.z = (c2.z - c1.z) * (v - vmin) / (ratio * dv) + c1.z;
      } else {
        c.x = (c3.x - c2.x) * (v - vmid) / ((1 - ratio) * dv) + c2.x;
        c.y = (c3.y - c2.y) * (v - vmid) / ((1 - ratio) * dv) + c2.y;
        c.z = (c3.z - c2.z) * (v - vmid) / ((1 - ratio) * dv) + c2.z;
      }
      break;
    case 13:
      c1.x = 0 / 255;
      c1.y = 255 / 255;
      c1.z = 0 / 255;
      c2.x = 255 / 255;
      c2.y = 150 / 255;
      c2.z = 0 / 255;
      c3.x = 255 / 255;
      c3.y = 250 / 255;
      c3.z = 240 / 255;
      ratio = 0.3f;
      vmid = vmin + ratio * dv;
      if (v < vmid) {
        c.x = (c2.x - c1.x) * (v - vmin) / (ratio * dv) + c1.x;
        c.y = (c2.y - c1.y) * (v - vmin) / (ratio * dv) + c1.y;
        c.z = (c2.z - c1.z) * (v - vmin) / (ratio * dv) + c1.z;
      } else {
        c.x = (c3.x - c2.x) * (v - vmid) / ((1 - ratio) * dv) + c2.x;
        c.y = (c3.y - c2.y) * (v - vmid) / ((1 - ratio) * dv) + c2.y;
        c.z = (c3.z - c2.z) * (v - vmid) / ((1 - ratio) * dv) + c2.z;
      }
      break;
    case 14:
      c.x = 1;
      c.y = 1 - (v - vmin) / dv;
      c.z = 0;
      break;
    case 15:
      if (v < (vmin + 0.25f * dv)) {
        c.x = 0;
        c.y = 4 * (v - vmin) / dv;
        c.z = 1;
      } else if (v < (vmin + 0.5f * dv)) {
        c.x = 0;
        c.y = 1;
        c.z = 1 - 4 * (v - vmin - 0.25f * dv) / dv;
      } else if (v < (vmin + 0.75f * dv)) {
        c.x = 4 * (v - vmin - 0.5f * dv) / dv;
        c.y = 1;
        c.z = 0;
      } else {
        c.x = 1;
        c.y = 1;
        c.z = 4 * (v - vmin - 0.75f * dv) / dv;
      }
      break;
    case 16:
      if (v < (vmin + 0.5f * dv)) {
        c.x = 0;
        c.y = 2 * (v - vmin) / dv;
        c.z = 1 - 2 * (v - vmin) / dv;
      } else {
        c.x = 2 * (v - vmin - 0.5f * dv) / dv;
        c.y = 1 - 2 * (v - vmin - 0.5f * dv) / dv;
        c.z = 0;
      }
      break;
    case 17:
      if (v < (vmin + 0.5f * dv)) {
        c.x = 1;
        c.y = 1 - 2 * (v - vmin) / dv;
        c.z = 2 * (v - vmin) / dv;
      } else {
        c.x = 1 - 2 * (v - vmin - 0.5f * dv) / dv;
        c.y = 2 * (v - vmin - 0.5f * dv) / dv;
        c.z = 1;
      }
      break;
    case 18:
      c.x = 0;
      c.y = (v - vmin) / (vmax - vmin);
      c.z = 1;
      break;
    case 19:
      c.x = (v - vmin) / (vmax - vmin);
      c.y = c.x;
      c.z = 1;
      break;
    case 20:
      c1.x = 0 / 255;
      c1.y = 160 / 255;
      c1.z = 0 / 255;
      c2.x = 180 / 255;
      c2.y = 220 / 255;
      c2.z = 0 / 255;
      c3.x = 250 / 255;
      c3.y = 220 / 255;
      c3.z = 170 / 255;
      ratio = 0.3f;
      vmid = vmin + ratio * dv;
      if (v < vmid) {
        c.x = (c2.x - c1.x) * (v - vmin) / (ratio * dv) + c1.x;
        c.y = (c2.y - c1.y) * (v - vmin) / (ratio * dv) + c1.y;
        c.z = (c2.z - c1.z) * (v - vmin) / (ratio * dv) + c1.z;
      } else {
        c.x = (c3.x - c2.x) * (v - vmid) / ((1 - ratio) * dv) + c2.x;
        c.y = (c3.y - c2.y) * (v - vmid) / ((1 - ratio) * dv) + c2.y;
        c.z = (c3.z - c2.z) * (v - vmid) / ((1 - ratio) * dv) + c2.z;
      }
      break;
    case 21:
      c1.x = 255 / 255;
      c1.y = 255 / 255;
      c1.z = 200 / 255;
      c2.x = 150 / 255;
      c2.y = 150 / 255;
      c2.z = 255 / 255;
      c.x = (c2.x - c1.x) * (v - vmin) / dv + c1.x;
      c.y = (c2.y - c1.y) * (v - vmin) / dv + c1.y;
      c.z = (c2.z - c1.z) * (v - vmin) / dv + c1.z;
      break;
    case 22:
      c.x = 1 - (v - vmin) / dv;
      c.y = 1 - (v - vmin) / dv;
      c.z = (v - vmin) / dv;
      break;
    case 23:
      if (v < (vmin + 0.5f * dv)) {
        c.x = 1;
        c.y = 2 * (v - vmin) / dv;
        c.z = c.y;
      } else {
        c.x = 1 - 2 * (v - vmin - 0.5f * dv) / dv;
        c.y = c.x;
        c.z = 1;
      }
      break;
    case 24:
      if (v < (vmin + 0.5f * dv)) {
        c.x = 2 * (v - vmin) / dv;
        c.y = c.x;
        c.z = 1 - c.x;
      } else {
        c.x = 1;
        c.y = 1 - 2 * (v - vmin - 0.5f * dv) / dv;
        c.z = 0;
      }
      break;
    case 25:
      if (v < (vmin + dv / 3)) {
        c.x = 0;
        c.y = 3 * (v - vmin) / dv;
        c.z = 1;
      } else if (v < (vmin + 2 * dv / 3)) {
        c.x = 3 * (v - vmin - dv / 3) / dv;
        c.y = 1 - c.x;
        c.z = 1;
      } else {
        c.x = 1;
        c.y = 0;
        c.z = 1 - 3 * (v - vmin - 2 * dv / 3) / dv;
      }
      break;
    }
    return c;
  }

  inline float abs_pow(double x, double y) { return pow(fabs(x), y); }

  vec3 calcPos(float theta, float phi) {
    float r = abs_pow(sin(code[0] * phi), code[1]) +
              abs_pow(cos(code[2] * phi), code[3]) +
              abs_pow(sin(code[4] * theta), code[5]) +
              abs_pow(cos(code[6] * theta), code[7]);

    // if (isnan(r)) r=0;
    return (vec3){r * sin(phi) * cos(theta), r * cos(phi),
                  r * sin(phi) * sin(theta)};
  }

  vec3 calcNormal(vec3 v0, vec3 v1, vec3 v2) {
    vec3 cr = vec3::cross(v2 - v0, v1 - v0);
    cr = cr.normalize();
    return cr.normalize();
  }

  void genCode(int nCode) {

    int m = SPHERICAL_HARMONICS_CODES[nCode % SH_N_CODES];

    for (int k = 0; k < 8; k++) {
      code[7 - k] = (m % 10);
      m /= 10;
    }
  }

  SH(int nCode, int resolution, int colorMap)
      : resolution(resolution), color_map(colorMap), n_code(nCode) {
    genCode(nCode);
    generate();
  }

  void generate() {
    mesh.resize(resolution * resolution * 4);
    Thread(resolution * resolution).run([this](int i) { genVertex(i); });
  }

  void genVertex(int index) {
    int i = index / resolution, j = index % resolution;

    double du = (M_PI * 2) / resolution, // Theta
        dv = M_PI / resolution,          // Phi
        dx = 1.f / resolution;
    double u = du * i, v = dv * j;

    vec3 position, normal;

    mesh[index * 4 + 0] = {.position = calcPos(u, v),
                           .normal = calcNormal(calcPos(u, v),
                                                calcPos(u + du / 10, v),
                                                calcPos(u, v + dv / 10)),
                           .color = calcColor(u, 0, M_PI * 2, color_map),
                           .texture = (vec3){i * dx, j * dx, 0}};

    mesh[index * 4 + 1] = {.position = calcPos(u + du, v),
                           .normal = calcNormal(calcPos(u + du, v),
                                                calcPos(u + du + du / 10, v),
                                                calcPos(u + du, v + dv / 10)),
                           .color = calcColor(u + du, 0, M_PI * 2, color_map),
                           .texture = (vec3){(i + 1) * dx, j * dx, 0}};
    mesh[index * 4 + 2] = {.position = calcPos(u + du, v + dv),
                           .normal =
                               calcNormal(calcPos(u + du, v + dv),
                                          calcPos(u + du + du / 10, v + dv),
                                          calcPos(u + du, v + dv + dv / 10)),
                           .color = calcColor(u + du, 0, M_PI * 2, color_map),
                           .texture = (vec3){(i + 1) * dx, (j + 1) * dx, 0}};
    mesh[index * 4 + 3] = {.position = calcPos(u, v + dv),
                           .normal = calcNormal(calcPos(u, v + dv),
                                                calcPos(u + du / 10, v + dv),
                                                calcPos(u, v + dv + dv / 10)),
                           .color = calcColor(u, 0, M_PI * 2, color_map),
                           .texture = (vec3){i * dx, (j + 1) * dx, 0}};
  }

  void paint() {
    int r = resolution;
    glBegin(GL_QUADS);
    for (int i = 0, index = 0; i < r; i++) {
      for (int j = 0; j < r; j++) {

        for (int k = 0; k < 4; k++, index++) {
          glNormal3dv((const GLdouble *)&mesh[index].normal);
          glColor3dv((const GLdouble *)&mesh[index].color);
          glTexCoord2dv((const GLdouble *)&mesh[index].texture);
          glVertex3dv((const GLdouble *)&mesh[index].position);
        }
      }
    }
    glEnd();
  }

  void createList(int ln) {
    glNewList(ln, GL_COMPILE);
    paint();
    glEndList();
    listCreated = true;
  }

  void draw(int ln = 6) {
    if (!listCreated)
      createList(ln);
    glCallList(ln);
  }

  static const int SH_N_CODES = 647;
  const int SPHERICAL_HARMONICS_CODES[SH_N_CODES] = {
      1222412,  1410121,  1420121,  1434301,  1441324,  1444401,  1444421,
      2222222,  2240322,  2420214,  2441224,  4026442,  4032241,  4240412,
      4310132,  4322142,  4323242,  4410112,  4422122,  4422133,  4422242,
      11111212, 11112242, 11121314, 11121442, 11121443, 11132444, 11134321,
      11142241, 11143234, 11214244, 11223344, 11224224, 11232334, 11242234,
      11244141, 11244224, 11244444, 11311232, 11314442, 11321224, 11321242,
      11331442, 11334422, 11344234, 11413142, 11421122, 11421133, 11421244,
      11422233, 11434241, 11441111, 11442211, 12121224, 12123222, 12123244,
      12124232, 12141212, 12221422, 12222212, 12222242, 12223242, 12244424,
      12320124, 12321244, 12322141, 12341234, 12414244, 12420224, 12420244,
      12421442, 12422232, 12431424, 12442124, 13121242, 13134224, 13142244,
      13224424, 13243234, 13312222, 13313342, 13324143, 13332424, 13342114,
      13422421, 13422421, 13434243, 13443212, 13443244, 13444124, 14032211,
      14122442, 14126211, 14131214, 14142242, 14222231, 14222414, 14234211,
      14234214, 14241424, 14242414, 14243444, 14322212, 14333242, 14344432,
      14414232, 14422143, 14431243, 14432424, 14434241, 14444122, 14444232,
      21022212, 21023122, 21030324, 21142223, 21142424, 21210412, 21212121,
      21213434, 21214422, 21222222, 21222422, 21224212, 21234314, 21332321,
      21333444, 21344422, 21412441, 21413214, 21413434, 21422122, 21422241,
      21442221, 22023304, 22024402, 22041224, 22113231, 22124144, 22133212,
      22141344, 22144344, 22212414, 22222244, 22223232, 22224231, 22224242,
      22232442, 22243224, 22243442, 22314442, 22323222, 22323322, 22334334,
      22344234, 22344404, 22411232, 22411432, 22420214, 22424222, 22424224,
      22431442, 22432424, 22442212, 22442344, 22443232, 23112442, 23124422,
      23124443, 23134234, 23142213, 23142314, 23143212, 23214221, 23224442,
      23230324, 23232322, 23242441, 23244133, 23312441, 23324424, 23332244,
      23344241, 23412342, 23414421, 23424144, 23432332, 23434423, 23442443,
      23444233, 23444312, 24024442, 24112332, 24124442, 24133441, 24134314,
      24144342, 24213423, 24222224, 24222422, 24222442, 24224422, 24234422,
      24241212, 24242142, 24242412, 24243434, 24244224, 24313124, 24324433,
      24330324, 24330324, 24333333, 24341423, 24412424, 24422214, 24422222,
      24423423, 24431212, 24442231, 24444222, 31112444, 31124442, 31132324,
      31142224, 31214244, 31221122, 31234431, 31244224, 31313422, 31323222,
      31331234, 31342434, 31344234, 31414234, 31422241, 31432221, 31434111,
      31434321, 31443224, 32111242, 32120214, 32123441, 32132224, 32144244,
      32220144, 32221214, 32224222, 32224244, 32231242, 32243234, 32314222,
      32321442, 32343222, 32412124, 32424232, 32424242, 32432124, 32432222,
      32441232, 33141232, 33221322, 33244232, 33333333, 33412244, 33421234,
      33422432, 33423121, 33441233, 34111244, 34124244, 34134243, 34143141,
      34143144, 34210144, 34223221, 34223244, 34224224, 34234324, 34241214,
      34243131, 34243212, 34314242, 34322112, 34334242, 34342414, 34343434,
      34414442, 34422142, 34423242, 34424334, 34431243, 34432241, 34441441,
      34442122, 34443234, 34444122, 41112442, 41122442, 41124122, 41132432,
      41142244, 41144141, 41144442, 41212121, 41213244, 41213422, 41224124,
      41224224, 41224334, 41231242, 41242214, 41244432, 41311222, 41313222,
      41313442, 41324211, 41334223, 41341222, 41341222, 41342214, 41344441,
      41412121, 41421442, 41422334, 41434144, 41442434, 42000024, 42024232,
      42111412, 42123241, 42131212, 42142244, 42212412, 42221124, 42221222,
      42222232, 42223432, 42232414, 42233223, 42241212, 42313422, 42323244,
      42323422, 42324244, 42333422, 42333442, 42342341, 42344241, 42412444,
      42413121, 42421424, 42422424, 42423232, 42424141, 42424444, 42433124,
      42441111, 42441222, 42441232, 42622462, 42624422, 43114443, 43122224,
      43124114, 43131324, 43134144, 43142212, 43144344, 43214321, 43221432,
      43232442, 43244322, 43313443, 43323212, 43323212, 43324224, 43334413,
      43342222, 43342432, 43344334, 43414422, 43421121, 43424242, 43434142,
      43434144, 43434442, 43444422, 44004400, 44112412, 44113231, 44121224,
      44134122, 44134324, 44143322, 44213242, 44221144, 44234124, 44234232,
      44243422, 44314123, 44322124, 44334242, 44334343, 44342232, 44342412,
      44414224, 44421242, 44421421, 44421424, 44431421, 44432424, 44441212,
      44444242, 12345678, 13287282, 26345664, 26722884, 27282827, 27382738,
      27384856, 34567812, 36178242, 36377284, 36383836, 36546644, 37483847,
      41828446, 42273881, 42428822, 42646246, 45226644, 45434666, 45544256,
      45565254, 45634566, 46266464, 46352226, 46466433, 46514416, 46544346,
      46544654, 46545253, 46611454, 46636546, 46727861, 46848126, 47484748,
      47626684, 48422614, 48424841, 51144446, 51263462, 51325455, 51446454,
      51546634, 51563652, 51616151, 51644243, 51644633, 52145236, 52222553,
      52344664, 52465354, 52466446, 52545256, 52564464, 52566465, 52664654,
      52824574, 52828252, 53164266, 53261146, 53364463, 53426426, 53464345,
      53536564, 53623456, 53634434, 53665364, 53816273, 54354662, 54365636,
      54424262, 54445464, 54466344, 54546444, 54613546, 54633426, 54644554,
      54647484, 55266536, 55446446, 55546256, 55555555, 55556666, 56266411,
      56344624, 56366644, 56434663, 56645264, 56646264, 57356365, 57386575,
      61144246, 61243256, 61345524, 61366442, 61446256, 61452663, 61465462,
      61465642, 61487462, 61515162, 61546264, 61555464, 61626364, 61644644,
      61645245, 62246654, 62446264, 62544564, 62545366, 62546455, 62624554,
      62648628, 62666461, 62726574, 63266454, 63286212, 63364224, 63366254,
      63446264, 62545564, 63626263, 63636266, 64162446, 64252546, 64354462,
      64365636, 64415264, 64436544, 64446264, 64446534, 64534244, 64636261,
      64644554, 64668571, 64828241, 65345261, 65432884, 65436543, 65446264,
      65526244, 65533264, 65536266, 65464838, 65784231, 65837244, 66162444,
      66226644, 66245544, 66344661, 66365254, 66444264, 66446264, 66446644,
      66526652, 66566424, 66576658, 66635246, 66644624, 66665656, 66666868,
      66872244, 67184718, 67442786, 67822674, 68166264, 68284821, 68426842,
      68626448, 68628448, 71288472, 71528364, 72484846, 72527252, 72727474,
      72737475, 72747678, 72774848, 72816384, 73747526, 73836283, 74737271,
      74846484, 75227641, 75318642, 75717472, 75737274, 76677484, 76737321,
      77447722, 77665544, 77784846, 78167264, 78332364, 78767684, 78787274,
      81417181, 81828281, 81828384, 82222534, 82246116, 82264224, 82624242,
      82645731, 82727282, 82747816, 82828484, 82848688, 83325375, 83737383,
      83828482, 83848483, 84622884, 84627181, 84627531, 84644221, 84682866,
      84822221, 84838281, 84841111, 85243642, 85737583, 85847372, 85848182,
      85858686, 85868283, 86442221, 86838321, 87766554, 88228822, 88646261,
      88824442, 88888888, 44444444};
};
