#include "SphericalHarmonics.h"

SphericalHarmonics::SphericalHarmonics() {
  loadCodes();
  ReadCode(-1); // random code
}

void SphericalHarmonics::ReadCode(
    int code) { // read a code from the set. default SEQ.
  int n = N_SH_CODES - 1, r;

  switch (code) {
  case -2: {
    static int c = 0;
    r = c % n;
    c++;
  }; break; // seq

  case -1: // random code
    r = rand() % n;
    break;
  default:
    r = code % n;
    break;
  }
  Code = SphericHarmCodes[r];
  for (int i = 0; i < 8; i++)
    m[i] = SphericHarmCodes[r][i] - '0';
  // new code
  listCreated = false;
}

// check pow 0^0
double SphericalHarmonics::powc(double x, double y) {
  if ((x < 0) || (x == 0 && y == 0))
    return 0;
  else
    return pow(x, y);
}

XYZ SphericalHarmonics::Eval(double theta, double phi) {
  double r = 0;
  XYZ p;

  r += powc(sin(m[0] * phi), m[1]);
  r += powc(cos(m[2] * phi), m[3]);
  r += powc(sin(m[4] * theta), m[5]);
  r += powc(cos(m[6] * theta), m[7]);

  p.x = r * sin(phi) * cos(theta);
  p.y = r * cos(phi);
  p.z = r * sin(phi) * sin(theta);

  return (p);
}

XYZ SphericalHarmonics::CalcNormal(XYZ p, XYZ p1, XYZ p2) {
  XYZ n, pa, pb;

  pa.x = p1.x - p.x;
  pa.y = p1.y - p.y;
  pa.z = p1.z - p.z;
  pb.x = p2.x - p.x;
  pb.y = p2.y - p.y;
  pb.z = p2.z - p.z;
  n.x = pa.y * pb.z - pa.z * pb.y;
  n.y = pa.z * pb.x - pa.x * pb.z;
  n.z = pa.x * pb.y - pa.y * pb.x;
  Normalise(&n);

  return (n);
}

void SphericalHarmonics::Normalise(XYZ *p) {
  double length;

  length = p->x * p->x + p->y * p->y + p->z * p->z;
  if (length > 0) {
    length = sqrt(length);
    p->x /= length;
    p->y /= length;
    p->z /= length;
  } else {
    p->x = 0;
    p->y = 0;
    p->z = 0;
  }
}

COLOUR SphericalHarmonics::GetColour(double v, double vmin, double vmax,
                                     int type) {
  double dv, vmid;
  COLOUR c = {1.0, 1.0, 1.0};
  COLOUR c1, c2, c3;
  double ratio;

  if (vmax < vmin) {
    dv = vmin;
    vmin = vmax;
    vmax = dv;
  }
  if (vmax - vmin < 0.000001) {
    vmin -= 1;
    vmax += 1;
  }
  if (v < vmin)
    v = vmin;
  if (v > vmax)
    v = vmax;
  dv = vmax - vmin;

  switch (type) {
  case 1:
    if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
      c.b = 1;
    } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.g = 1;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
    } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.g = 1;
      c.b = 0;
    } else {
      c.r = 1;
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
    }
    break;
  case 2:
    c.r = (v - vmin) / dv;
    c.g = 0;
    c.b = (vmax - v) / dv;
    break;
  case 3:
    c.r = (v - vmin) / dv;
    c.b = c.r;
    c.g = c.r;
    break;
  case 4:
    if (v < (vmin + dv / 6.0)) {
      c.r = 1;
      c.g = 6 * (v - vmin) / dv;
      c.b = 0;
    } else if (v < (vmin + 2.0 * dv / 6.0)) {
      c.r = 1 + 6 * (vmin + dv / 6.0 - v) / dv;
      c.g = 1;
      c.b = 0;
    } else if (v < (vmin + 3.0 * dv / 6.0)) {
      c.r = 0;
      c.g = 1;
      c.b = 6 * (v - vmin - 2.0 * dv / 6.0) / dv;
    } else if (v < (vmin + 4.0 * dv / 6.0)) {
      c.r = 0;
      c.g = 1 + 6 * (vmin + 3.0 * dv / 6.0 - v) / dv;
      c.b = 1;
    } else if (v < (vmin + 5.0 * dv / 6.0)) {
      c.r = 6 * (v - vmin - 4.0 * dv / 6.0) / dv;
      c.g = 0;
      c.b = 1;
    } else {
      c.r = 1;
      c.g = 0;
      c.b = 1 + 6 * (vmin + 5.0 * dv / 6.0 - v) / dv;
    }
    break;
  case 5:
    c.r = (v - vmin) / (vmax - vmin);
    c.g = 1;
    c.b = 0;
    break;
  case 6:
    c.r = (v - vmin) / (vmax - vmin);
    c.g = (vmax - v) / (vmax - vmin);
    c.b = c.r;
    break;
  case 7:
    if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
      c.b = 1 - c.g;
    } else if (v < (vmin + 0.5 * dv)) {
      c.r = 4 * (v - vmin - 0.25 * dv) / dv;
      c.g = 1 - c.r;
      c.b = 0;
    } else if (v < (vmin + 0.75 * dv)) {
      c.g = 4 * (v - vmin - 0.5 * dv) / dv;
      c.r = 1 - c.g;
      c.b = 0;
    } else {
      c.r = 0;
      c.b = 4 * (v - vmin - 0.75 * dv) / dv;
      c.g = 1 - c.b;
    }
    break;
  case 8:
    if (v < (vmin + 0.5 * dv)) {
      c.r = 2 * (v - vmin) / dv;
      c.g = c.r;
      c.b = c.r;
    } else {
      c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
      c.g = c.r;
      c.b = c.r;
    }
    break;
  case 9:
    if (v < (vmin + dv / 3)) {
      c.b = 3 * (v - vmin) / dv;
      c.g = 0;
      c.r = 1 - c.b;
    } else if (v < (vmin + 2 * dv / 3)) {
      c.r = 0;
      c.g = 3 * (v - vmin - dv / 3) / dv;
      c.b = 1;
    } else {
      c.r = 3 * (v - vmin - 2 * dv / 3) / dv;
      c.g = 1 - c.r;
      c.b = 1;
    }
    break;
  case 10:
    if (v < (vmin + 0.2 * dv)) {
      c.r = 0;
      c.g = 5 * (v - vmin) / dv;
      c.b = 1;
    } else if (v < (vmin + 0.4 * dv)) {
      c.r = 0;
      c.g = 1;
      c.b = 1 + 5 * (vmin + 0.2 * dv - v) / dv;
    } else if (v < (vmin + 0.6 * dv)) {
      c.r = 5 * (v - vmin - 0.4 * dv) / dv;
      c.g = 1;
      c.b = 0;
    } else if (v < (vmin + 0.8 * dv)) {
      c.r = 1;
      c.g = 1 - 5 * (v - vmin - 0.6 * dv) / dv;
      c.b = 0;
    } else {
      c.r = 1;
      c.g = 5 * (v - vmin - 0.8 * dv) / dv;
      c.b = 5 * (v - vmin - 0.8 * dv) / dv;
    }
    break;
  case 11:
    c1.r = 200 / 255.0;
    c1.g = 60 / 255.0;
    c1.b = 0 / 255.0;
    c2.r = 250 / 255.0;
    c2.g = 160 / 255.0;
    c2.b = 110 / 255.0;
    c.r = (c2.r - c1.r) * (v - vmin) / dv + c1.r;
    c.g = (c2.g - c1.g) * (v - vmin) / dv + c1.g;
    c.b = (c2.b - c1.b) * (v - vmin) / dv + c1.b;
    break;
  case 12:
    c1.r = 55 / 255.0;
    c1.g = 55 / 255.0;
    c1.b = 45 / 255.0;
    /* c2.r = 200 / 255.0; c2.g =  60 / 255.0; c2.b =   0 / 255.0; */
    c2.r = 235 / 255.0;
    c2.g = 90 / 255.0;
    c2.b = 30 / 255.0;
    c3.r = 250 / 255.0;
    c3.g = 160 / 255.0;
    c3.b = 110 / 255.0;
    ratio = 0.4;
    vmid = vmin + ratio * dv;
    if (v < vmid) {
      c.r = (c2.r - c1.r) * (v - vmin) / (ratio * dv) + c1.r;
      c.g = (c2.g - c1.g) * (v - vmin) / (ratio * dv) + c1.g;
      c.b = (c2.b - c1.b) * (v - vmin) / (ratio * dv) + c1.b;
    } else {
      c.r = (c3.r - c2.r) * (v - vmid) / ((1 - ratio) * dv) + c2.r;
      c.g = (c3.g - c2.g) * (v - vmid) / ((1 - ratio) * dv) + c2.g;
      c.b = (c3.b - c2.b) * (v - vmid) / ((1 - ratio) * dv) + c2.b;
    }
    break;
  case 13:
    c1.r = 0 / 255.0;
    c1.g = 255 / 255.0;
    c1.b = 0 / 255.0;
    c2.r = 255 / 255.0;
    c2.g = 150 / 255.0;
    c2.b = 0 / 255.0;
    c3.r = 255 / 255.0;
    c3.g = 250 / 255.0;
    c3.b = 240 / 255.0;
    ratio = 0.3;
    vmid = vmin + ratio * dv;
    if (v < vmid) {
      c.r = (c2.r - c1.r) * (v - vmin) / (ratio * dv) + c1.r;
      c.g = (c2.g - c1.g) * (v - vmin) / (ratio * dv) + c1.g;
      c.b = (c2.b - c1.b) * (v - vmin) / (ratio * dv) + c1.b;
    } else {
      c.r = (c3.r - c2.r) * (v - vmid) / ((1 - ratio) * dv) + c2.r;
      c.g = (c3.g - c2.g) * (v - vmid) / ((1 - ratio) * dv) + c2.g;
      c.b = (c3.b - c2.b) * (v - vmid) / ((1 - ratio) * dv) + c2.b;
    }
    break;
  case 14:
    c.r = 1;
    c.g = 1 - (v - vmin) / dv;
    c.b = 0;
    break;
  case 15:
    if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
      c.b = 1;
    } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.g = 1;
      c.b = 1 - 4 * (v - vmin - 0.25 * dv) / dv;
    } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.g = 1;
      c.b = 0;
    } else {
      c.r = 1;
      c.g = 1;
      c.b = 4 * (v - vmin - 0.75 * dv) / dv;
    }
    break;
  case 16:
    if (v < (vmin + 0.5 * dv)) {
      c.r = 0.0;
      c.g = 2 * (v - vmin) / dv;
      c.b = 1 - 2 * (v - vmin) / dv;
    } else {
      c.r = 2 * (v - vmin - 0.5 * dv) / dv;
      c.g = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0.0;
    }
    break;
  case 17:
    if (v < (vmin + 0.5 * dv)) {
      c.r = 1.0;
      c.g = 1 - 2 * (v - vmin) / dv;
      c.b = 2 * (v - vmin) / dv;
    } else {
      c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
      c.g = 2 * (v - vmin - 0.5 * dv) / dv;
      c.b = 1.0;
    }
    break;
  case 18:
    c.r = 0;
    c.g = (v - vmin) / (vmax - vmin);
    c.b = 1;
    break;
  case 19:
    c.r = (v - vmin) / (vmax - vmin);
    c.g = c.r;
    c.b = 1;
    break;
  case 20:
    c1.r = 0 / 255.0;
    c1.g = 160 / 255.0;
    c1.b = 0 / 255.0;
    c2.r = 180 / 255.0;
    c2.g = 220 / 255.0;
    c2.b = 0 / 255.0;
    c3.r = 250 / 255.0;
    c3.g = 220 / 255.0;
    c3.b = 170 / 255.0;
    ratio = 0.3;
    vmid = vmin + ratio * dv;
    if (v < vmid) {
      c.r = (c2.r - c1.r) * (v - vmin) / (ratio * dv) + c1.r;
      c.g = (c2.g - c1.g) * (v - vmin) / (ratio * dv) + c1.g;
      c.b = (c2.b - c1.b) * (v - vmin) / (ratio * dv) + c1.b;
    } else {
      c.r = (c3.r - c2.r) * (v - vmid) / ((1 - ratio) * dv) + c2.r;
      c.g = (c3.g - c2.g) * (v - vmid) / ((1 - ratio) * dv) + c2.g;
      c.b = (c3.b - c2.b) * (v - vmid) / ((1 - ratio) * dv) + c2.b;
    }
    break;
  case 21:
    c1.r = 255 / 255.0;
    c1.g = 255 / 255.0;
    c1.b = 200 / 255.0;
    c2.r = 150 / 255.0;
    c2.g = 150 / 255.0;
    c2.b = 255 / 255.0;
    c.r = (c2.r - c1.r) * (v - vmin) / dv + c1.r;
    c.g = (c2.g - c1.g) * (v - vmin) / dv + c1.g;
    c.b = (c2.b - c1.b) * (v - vmin) / dv + c1.b;
    break;
  case 22:
    c.r = 1 - (v - vmin) / dv;
    c.g = 1 - (v - vmin) / dv;
    c.b = (v - vmin) / dv;
    break;
  case 23:
    if (v < (vmin + 0.5 * dv)) {
      c.r = 1;
      c.g = 2 * (v - vmin) / dv;
      c.b = c.g;
    } else {
      c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
      c.g = c.r;
      c.b = 1;
    }
    break;
  case 24:
    if (v < (vmin + 0.5 * dv)) {
      c.r = 2 * (v - vmin) / dv;
      c.g = c.r;
      c.b = 1 - c.r;
    } else {
      c.r = 1;
      c.g = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
    }
    break;
  case 25:
    if (v < (vmin + dv / 3)) {
      c.r = 0;
      c.g = 3 * (v - vmin) / dv;
      c.b = 1;
    } else if (v < (vmin + 2 * dv / 3)) {
      c.r = 3 * (v - vmin - dv / 3) / dv;
      c.g = 1 - c.r;
      c.b = 1;
    } else {
      c.r = 1;
      c.g = 0;
      c.b = 1 - 3 * (v - vmin - 2 * dv / 3) / dv;
    }
    break;
  }
  return (c);
}

void SphericalHarmonics::Paint(void) {
  double du, dv, u, v, dx;
  XYZ q[4], n[4];
  COLOUR c[4];

  vertexes.clear();
  normals.clear();
  colors.clear();

  cnt++;

  du = TWOPI / (double)resolution; /* Theta */
  dv = PI / (double)resolution;    /* Phi */
  dx = 1 / (double)resolution;

  glBegin(GL_QUADS);
  // GL_TRIANGLE_FAN); //GL_POINTS); //GL_TRIANGLES); //GL_LINES); //);
  for (int i = 0; i < resolution; i++) {
    u = i * du;
    for (int j = 0; j < resolution; j++) {
      v = j * dv;
      q[0] = Eval(u, v);
      n[0] = CalcNormal(q[0], Eval(u + du / 10, v), Eval(u, v + dv / 10));
      c[0] = GetColour(u, 0.0, TWOPI, colourmap);
      glNormal3f(n[0].x, n[0].y, n[0].z);
      glColor3f(c[0].r, c[0].g, c[0].b);
      glTexCoord2f(i * dx, j * dx);
      glVertex3f(q[0].x, q[0].y, q[0].z);

      q[1] = Eval(u + du, v);
      n[1] = CalcNormal(q[1], Eval(u + du + du / 10, v),
                        Eval(u + du, v + dv / 10));
      c[1] = GetColour(u + du, 0.0, TWOPI, colourmap);
      glNormal3f(n[1].x, n[1].y, n[1].z);
      glColor3f(c[1].r, c[1].g, c[1].b);
      glVertex3f(q[1].x, q[1].y, q[1].z);
      glTexCoord2f((i + 1) * dx, j * dx);

      q[2] = Eval(u + du, v + dv);
      n[2] = CalcNormal(q[2], Eval(u + du + du / 10, v + dv),
                        Eval(u + du, v + dv + dv / 10));
      c[2] = GetColour(u + du, 0.0, TWOPI, colourmap);
      glNormal3f(n[2].x, n[2].y, n[2].z);
      glColor3f(c[2].r, c[2].g, c[2].b);
      glVertex3f(q[2].x, q[2].y, q[2].z);
      glTexCoord2f((i + 1) * dx, (j + 1) * dx);

      q[3] = Eval(u, v + dv);
      n[3] = CalcNormal(q[3], Eval(u + du / 10, v + dv),
                        Eval(u, v + dv + dv / 10));
      c[3] = GetColour(u, 0.0, TWOPI, colourmap);
      glNormal3f(n[3].x, n[3].y, n[3].z);
      glColor3f(c[3].r, c[3].g, c[3].b);
      glVertex3f(q[3].x, q[3].y, q[3].z);
      glTexCoord2f(i * dx, (j + 1) * dx);

      { // save mesh res x res
        vertexes.push_back(q[0]);
        normals.push_back(n[0]);
        colors.push_back(c[0]);
      }
    }
  }

  glEnd();
}

// create the list '1' by default
void SphericalHarmonics::createList(int ln) {
  glNewList(ln, GL_COMPILE);
  Paint();
  glEndList();
  listCreated = true;
}

void SphericalHarmonics::draw(int ln) {
  if (!listCreated)
    createList(ln);
  glCallList(ln);
}

void SphericalHarmonics::Light(void) { // default light
  static GLfloat LightAmbient[] = {1, 1, 1, 0};
  static GLfloat LightDiffuse[] = {1, 0.6, 0.8, 0}; // defines light's colour
  static GLfloat LightPosition[] = {10, 20, 30, 0};

  Light(GL_LIGHT0, LightAmbient, LightDiffuse, LightPosition);
}

void SphericalHarmonics::Light1(void) { // default light
  static GLfloat LightAmbient[] = {0.8, 0.3, 0, 0};
  static GLfloat LightDiffuse[] = {0.4, 0.2, 0, 0}; // defines light's colour
  static GLfloat LightPosition[] = {-10, 20, 30, 0};

  Light(GL_LIGHT1, LightAmbient, LightDiffuse, LightPosition);
}

void SphericalHarmonics::Light(GLenum nlight, const GLfloat *LightAmbient,
                               const GLfloat *LightDiffuse,
                               const GLfloat *LightPosition) {
  glEnable(GL_LIGHTING);
  glEnable(nlight);                              // Enable Light
  glLightfv(nlight, GL_AMBIENT, LightAmbient);   // Setup The Ambient Light
  glLightfv(nlight, GL_DIFFUSE, LightDiffuse);   // Setup The Diffuse Light
  glLightfv(nlight, GL_POSITION, LightPosition); // Position The Light
}

void SphericalHarmonics::loadCodes() {
  static string __SphericHarmCodes[N_SH_CODES] = {
      "01222412", "01410121", "01420121", "01434301", "01441324", "01444401",
      "01444421", "02222222", "02240322", "02420214", "02441224", "04026442",
      "04032241", "04240412", "04310132", "04322142", "04323242", "04410112",
      "04422122", "04422133", "04422242", "11111212", "11112242", "11121314",
      "11121442", "11121443", "11132444", "11134321", "11142241", "11143234",
      "11214244", "11223344", "11224224", "11232334", "11242234", "11244141",
      "11244224", "11244444", "11311232", "11314442", "11321224", "11321242",
      "11331442", "11334422", "11344234", "11413142", "11421122", "11421133",
      "11421244", "11422233", "11434241", "11441111", "11442211", "12121224",
      "12123222", "12123244", "12124232", "12141212", "12221422", "12222212",
      "12222242", "12223242", "12244424", "12320124", "12321244", "12322141",
      "12341234", "12414244", "12420224", "12420244", "12421442", "12422232",
      "12431424", "12442124", "13121242", "13134224", "13142244", "13224424",
      "13243234", "13312222", "13313342", "13324143", "13332424", "13342114",
      "13422421", "13422421", "13434243", "13443212", "13443244", "13444124",
      "14032211", "14122442", "14126211", "14131214", "14142242", "14222231",
      "14222414", "14234211", "14234214", "14241424", "14242414", "14243444",
      "14322212", "14333242", "14344432", "14414232", "14422143", "14431243",
      "14432424", "14434241", "14444122", "14444232", "21022212", "21023122",
      "21030324", "21142223", "21142424", "21210412", "21212121", "21213434",
      "21214422", "21222222", "21222422", "21224212", "21234314", "21332321",
      "21333444", "21344422", "21412441", "21413214", "21413434", "21422122",
      "21422241", "21442221", "22023304", "22024402", "22041224", "22113231",
      "22124144", "22133212", "22141344", "22144344", "22212414", "22222244",
      "22223232", "22224231", "22224242", "22232442", "22243224", "22243442",
      "22314442", "22323222", "22323322", "22334334", "22344234", "22344404",
      "22411232", "22411432", "22420214", "22424222", "22424224", "22431442",
      "22432424", "22442212", "22442344", "22443232", "23112442", "23124422",
      "23124443", "23134234", "23142213", "23142314", "23143212", "23214221",
      "23224442", "23230324", "23232322", "23242441", "23244133", "23312441",
      "23324424", "23332244", "23344241", "23412342", "23414421", "23424144",
      "23432332", "23434423", "23442443", "23444233", "23444312", "24024442",
      "24112332", "24124442", "24133441", "24134314", "24144342", "24213423",
      "24222224", "24222422", "24222442", "24224422", "24234422", "24241212",
      "24242142", "24242412", "24243434", "24244224", "24313124", "24324433",
      "24330324", "24330324", "24333333", "24341423", "24412424", "24422214",
      "24422222", "24423423", "24431212", "24442231", "24444222", "31112444",
      "31124442", "31132324", "31142224", "31214244", "31221122", "31234431",
      "31244224", "31313422", "31323222", "31331234", "31342434", "31344234",
      "31414234", "31422241", "31432221", "31434111", "31434321", "31443224",
      "32111242", "32120214", "32123441", "32132224", "32144244", "32220144",
      "32221214", "32224222", "32224244", "32231242", "32243234", "32314222",
      "32321442", "32343222", "32412124", "32424232", "32424242", "32432124",
      "32432222", "32441232", "33141232", "33221322", "33244232", "33333333",
      "33412244", "33421234", "33422432", "33423121", "33441233", "34111244",
      "34124244", "34134243", "34143141", "34143144", "34210144", "34223221",
      "34223244", "34224224", "34234324", "34241214", "34243131", "34243212",
      "34314242", "34322112", "34334242", "34342414", "34343434", "34414442",
      "34422142", "34423242", "34424334", "34431243", "34432241", "34441441",
      "34442122", "34443234", "34444122", "41112442", "41122442", "41124122",
      "41132432", "41142244", "41144141", "41144442", "41212121", "41213244",
      "41213422", "41224124", "41224224", "41224334", "41231242", "41242214",
      "41244432", "41311222", "41313222", "41313442", "41324211", "41334223",
      "41341222", "41341222", "41342214", "41344441", "41412121", "41421442",
      "41422334", "41434144", "41442434", "42000024", "42024232", "42111412",
      "42123241", "42131212", "42142244", "42212412", "42221124", "42221222",
      "42222232", "42223432", "42232414", "42233223", "42241212", "42313422",
      "42323244", "42323422", "42324244", "42333422", "42333442", "42342341",
      "42344241", "42412444", "42413121", "42421424", "42422424", "42423232",
      "42424141", "42424444", "42433124", "42441111", "42441222", "42441232",
      "42622462", "42624422", "43114443", "43122224", "43124114", "43131324",
      "43134144", "43142212", "43144344", "43214321", "43221432", "43232442",
      "43244322", "43313443", "43323212", "43323212", "43324224", "43334413",
      "43342222", "43342432", "43344334", "43414422", "43421121", "43424242",
      "43434142", "43434144", "43434442", "43444422", "44004400", "44112412",
      "44113231", "44121224", "44134122", "44134324", "44143322", "44213242",
      "44221144", "44234124", "44234232", "44243422", "44314123", "44322124",
      "44334242", "44334343", "44342232", "44342412", "44414224", "44421242",
      "44421421", "44421424", "44431421", "44432424", "44441212", "44444242",
      "12345678", "13287282", "26345664", "26722884", "27282827", "27382738",
      "27384856", "34567812", "36178242", "36377284", "36383836", "36546644",
      "37483847", "41828446", "42273881", "42428822", "42646246", "45226644",
      "45434666", "45544256", "45565254", "45634566", "46266464", "46352226",
      "46466433", "46514416", "46544346", "46544654", "46545253", "46611454",
      "46636546", "46727861", "46848126", "47484748", "47626684", "48422614",
      "48424841", "51144446", "51263462", "51325455", "51446454", "51546634",
      "51563652", "51616151", "51644243", "51644633", "52145236", "52222553",
      "52344664", "52465354", "52466446", "52545256", "52564464", "52566465",
      "52664654", "52824574", "52828252", "53164266", "53261146", "53364463",
      "53426426", "53464345", "53536564", "53623456", "53634434", "53665364",
      "53816273", "54354662", "54365636", "54424262", "54445464", "54466344",
      "54546444", "54613546", "54633426", "54644554", "54647484", "55266536",
      "55446446", "55546256", "55555555", "55556666", "56266411", "56344624",
      "56366644", "56434663", "56645264", "56646264", "57356365", "57386575",
      "61144246", "61243256", "61345524", "61366442", "61446256", "61452663",
      "61465462", "61465642", "61487462", "61515162", "61546264", "61555464",
      "61626364", "61644644", "61645245", "62246654", "62446264", "62544564",
      "62545366", "62546455", "62624554", "62648628", "62666461", "62726574",
      "63266454", "63286212", "63364224", "63366254", "63446264", "62545564",
      "63626263", "63636266", "64162446", "64252546", "64354462", "64365636",
      "64415264", "64436544", "64446264", "64446534", "64534244", "64636261",
      "64644554", "64668571", "64828241", "65345261", "65432884", "65436543",
      "65446264", "65526244", "65533264", "65536266", "65464838", "65784231",
      "65837244", "66162444", "66226644", "66245544", "66344661", "66365254",
      "66444264", "66446264", "66446644", "66526652", "66566424", "66576658",
      "66635246", "66644624", "66665656", "66666868", "66872244", "67184718",
      "67442786", "67822674", "68166264", "68284821", "68426842", "68626448",
      "68628448", "71288472", "71528364", "72484846", "72527252", "72727474",
      "72737475", "72747678", "72774848", "72816384", "73747526", "73836283",
      "74737271", "74846484", "75227641", "75318642", "75717472", "75737274",
      "76677484", "76737321", "77447722", "77665544", "77784846", "78167264",
      "78332364", "78767684", "78787274", "81417181", "81828281", "81828384",
      "82222534", "82246116", "82264224", "82624242", "82645731", "82727282",
      "82747816", "82828484", "82848688", "83325375", "83737383", "83828482",
      "83848483", "84622884", "84627181", "84627531", "84644221", "84682866",
      "84822221", "84838281", "84841111", "85243642", "85737583", "85847372",
      "85848182", "85858686", "85868283", "86442221", "86838321", "87766554",
      "88228822", "88646261", "88824442", "88888888", "44444444"};
  SphericHarmCodes = __SphericHarmCodes;
}
