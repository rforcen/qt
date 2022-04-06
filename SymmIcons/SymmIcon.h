#pragma once

#include <QRectF>
#include <math.h>
#include <vector>

using std::vector;

static double presets[][7] =
    { // lambda, alpha, beta, gamma, omega, symmetry, scale
        {1.56, -1, 0.1, -0.82, -0.3, 3, 1.7},
        {-1.806, 1.806, 0, 1.5, 0, 7, 1.1},
        {2.4, -2.5, -0.9, 0.9, 0, 3, 1.5},
        {-2.7, 5, 1.5, 1, 0, 4, 1},
        {-2.5, 8, -0.7, 1, 0, 5, 0.8},
        {-1.9, 1.806, -0.85, 1.8, 0, 7, 1.2},
        {2.409, -2.5, 0, 0.9, 0, 4, 1.4},
        {-1.806, 1.807, -0.07, 1.08, 0, 6, 1.2},
        {-2.34, 2.2, 0.4, 0.05, 0, 5, 1.2},
        {-2.57, 3.2, 1.2, -1.75, 0, 36, 1.2},
        {-2.6, 4, 1.5, 1, 0, 12, 1.1},
        {-2.2, 2.3, 0.55, -0.90, 0, 3, 1.3},
        {-2.205, 6.01, 13.5814, -0.2044, 0.011, 5, 0.8},
        {-2.7, 8.7, 13.86, -0.13, -0.18, 18, .8},
        {-2.52, 8.75, 12, 0.04, 0.18, 5, .8},
        {2.38, -4.18, 19.99, -0.69, 0.095, 17, 1},
        {2.33, -8.22, -6.07, -0.52, 0.16, 4, .8},
        {-1.62, 2.049, 1.422, 1.96, 0.56, 6, 1},
        {-1.89, 9.62, 1.95, 0.51, 0.21, 3, .6},
        {-1.65, 9.99, 1.57, 1.46, -0.55, 3, .8},
        {-2.7, 5, 1.5, 1, 0, 6, 1},
        {-2.08, 1, -.1, .167, 0, 7, 1.3},
        {1.56, -1, .1, -.82, .12, 3, 1.6},
        {-1.806, 1.806, 0, 1, 0, 5, 1.1},
        {1.56, -1, .1, -.82, 0, 3, 1.3},
        {-2.195, 10, -12, 1, 0, 3, .7},
        {-1.86, 2, 0, 1, .1, 4, 1.2},
        {-2.34, 2, .2, .1, 0, 5, 1.2},
        {2.6, -2, 0, .5, 0, 5, 1.3},
        {-2.5, 5, -1.9, 1, .188, 5, 1},
        {2.409, -2.5, 0, .9, 0, 23, 1.2},
        {2.409, -2.5, -.2, .81, 0, 24, 1.2},
        {-2.05, 3, -16.79, 1, 0, 9, 1},
        {-2.32, 2.32, 0, .75, 0, 5, 1.2},
        {2.5, -2.5, 0, .9, 0, 3, 1.3},
        {1.5, -1, .1, -.805, 0, 3, 1.4}};

class SymmetricIcon {

  const double MaxXY = 1e5;

public:
  double *preset = (double *)presets;
  int w = 0, h = 0;
  vector<int> screen;
  int iter;                                 // current iteration
  int npr = 0;                              // preset number
  double lambda, alpha, beta, gamma, omega; // icon parameters
  int sym;
  double apcx, apcy, rad;
  double scale; // 1.5;
  QRectF clipR;
  vector<int> ColorList;
  int speed = 100;
  vector<short> icon;
  bool isClick = false; // Graphics gr;
  int nPreset = sizeof(presets) / (7 * sizeof(double));
  double x, y;
  int k;

  SymmetricIcon(int npr = 0) : npr(npr) { setPreset(npr); }

  void setSize(int w, int h) {
    this->w = w;
    this->h = h;

    screen.resize((w + 2) * (h + 2));
    icon.resize((w + 2) * (h + 2));

    speed = 100;

    reset();
  }

  inline int coord1(int x, int y) { // convert 2d -> 1d coord
    return x + y * w;
  }

  bool iterMod(int mod) { return iter % mod == 0; }

  inline int argb(int a, int r, int g, int b) {
    return a << 24 | b << 16 | g << 8 | r;
  }

  inline int makeColor(int r, int g, int b) {
    return b << 16 | g << 8 | r | 0xff000000;
  }

  inline int makeColor(int a, int r, int g, int b) {
    return a << 24 | b << 16 | g << 8 | r;
  }

  void setColors(int paramInt) {
    vector<int> arrayOfColor(2112);

    int i, j, localColor;
    if (paramInt > 5) {
      switch (paramInt) {
      case 6:
        for (i = 0; i < 256; i++)
          arrayOfColor[(i + 64)] = makeColor(255, 255 - i, 255 - i, 255);
        break;
      case 7:
        for (i = 0; i < 256; i++)
          arrayOfColor[(i + 64)] = makeColor(255 - i, 255, 255);
        break;
      case 8:
        for (i = 0; i < 256; i++)
          arrayOfColor[(i + 64)] = makeColor(255 - i, 255, 255 - i);
        break;
      case 9:
        for (i = 0; i < 256; i++)
          arrayOfColor[(i + 64)] = makeColor(255, 255, 255 - i);
        break;
      case 10:
        for (i = 0; i < 256; i++)
          arrayOfColor[(i + 64)] = makeColor(255, 255 - i, 255 - i);
        break;
      case 11:
        i = 0;
        while (true) {
          arrayOfColor[(i + 64)] = makeColor(255, 255 - i, 255);
          i++;
          if (i >= 256)
            break;
        }
      }
      for (i = 0; i < 64; i++)
        arrayOfColor[i] = makeColor(4 * i, 4 * i, 4 * i);
      for (j = 0; j < 5; j++) {
        for (i = 0; i < 256; i++)
          arrayOfColor[(320 + j * 256 + i)] = getRainbow((paramInt + j) % 6, i);
      }
      for (i = 0; i < 256; i++) {
        localColor = getRainbow((paramInt - 1) % 6, i);
        arrayOfColor[(1600 + 2 * i)] = localColor;
        arrayOfColor[(1601 + 2 * i)] = localColor;
      }
    } else {
      switch (paramInt) {
      case 0:
        for (i = 0; i < 64; i++)
          arrayOfColor[i] = makeColor(0, 0, 4 * i);
        for (i = 0; i < 256; i++) {
          localColor = makeColor(255, i, 255);
          arrayOfColor[(1344 + 3 * i)] = localColor;
          arrayOfColor[(1345 + 3 * i)] = localColor;
          arrayOfColor[(1346 + 3 * i)] = localColor;
        }
        break;
      case 1:
        for (i = 0; i < 64; i++)
          arrayOfColor[i] = makeColor(0, 4 * i, 4 * i);
        for (i = 0; i < 256; i++) {
          localColor = makeColor(i, i, 255);
          arrayOfColor[(1344 + 3 * i)] = localColor;
          arrayOfColor[(1345 + 3 * i)] = localColor;
          arrayOfColor[(1346 + 3 * i)] = localColor;
        }
        break;
      case 2:
        for (i = 0; i < 64; i++)
          arrayOfColor[i] = makeColor(0, 4 * i, 0);
        for (i = 0; i < 256; i++) {
          localColor = makeColor(i, 255, 255);
          arrayOfColor[(1344 + 3 * i)] = localColor;
          arrayOfColor[(1345 + 3 * i)] = localColor;
          arrayOfColor[(1346 + 3 * i)] = localColor;
        }
        break;
      case 3:
        for (i = 0; i < 64; i++)
          arrayOfColor[i] = makeColor(4 * i, 4 * i, 0);
        for (i = 0; i < 256; i++) {
          localColor = makeColor(i, 255, i);
          arrayOfColor[(1344 + 3 * i)] = localColor;
          arrayOfColor[(1345 + 3 * i)] = localColor;
          arrayOfColor[(1346 + 3 * i)] = localColor;
        }
        break;
      case 4:
        for (i = 0; i < 64; i++)
          arrayOfColor[i] = makeColor(4 * i, 0, 0);
        for (i = 0; i < 256; i++) {
          localColor = makeColor(255, 255, i);
          arrayOfColor[(1344 + 3 * i)] = localColor;
          arrayOfColor[(1345 + 3 * i)] = localColor;
          arrayOfColor[(1346 + 3 * i)] = localColor;
        }
        break;
      case 5:
        for (i = 0; i < 64; i++)
          arrayOfColor[i] = makeColor(4 * i, 0, 4 * i);
        i = 0;
        while (true) {
          localColor = makeColor(255, i, i);
          arrayOfColor[(1344 + 3 * i)] = localColor;
          arrayOfColor[(1345 + 3 * i)] = localColor;
          arrayOfColor[(1346 + 3 * i)] = localColor;

          i++;
          if (i >= 256) {
            break;
          }
        }
      }
      for (j = 0; j < 5; j++) {
        for (i = 0; i < 256; i++)
          arrayOfColor[(64 + j * 256 + i)] = getRainbow((paramInt + j) % 6, i);
      }
    }
    ColorList = arrayOfColor;
  }

  int getRainbow(int x, int y) {
    switch (x) {
    case 0:
      return makeColor(0, y, 255);
    case 1:
      return makeColor(0, 255, 255 - y);
    case 2:
      return makeColor(y, 255, 0);
    case 3:
      return makeColor(255, 255 - y, 0);
    case 4:
      return makeColor(255, 0, y);
    case 5:
      return makeColor(255 - y, 0, 255);
    }
    return makeColor(0, 0, 0); // black
  }

  int getColor(int col) {
    if (ColorList.size() == 0)
      setColors(0);
    col &= 0x00ffffff;
    if (col * speed > 2111) {
      for (; (col * speed > 3071) && (speed > 3); --speed)
        ;
      return ColorList[2111];
    }
    return ColorList[col * speed];
  }

  void reset() {
    if (w && h) {
      speed = 100;

      apcx = (w / 2.0);
      apcy = (h / 2.0);
      rad = (apcx > apcy ? apcy : apcx);
      k = 0;
      x = 0.01;
      y = 0.003;

      for (int i = 0; i < h * w; i++)
        icon[i] = screen[i] = 0;

      clipR = QRectF(0, 0, w, h);

      for (int m = clipR.left(); (m < clipR.left() + clipR.width()) && (m < w);
           m++)
        for (int n = clipR.top(); (n < clipR.top() + clipR.height()) && (n < h);
             n++) {
          setPoint(m, n, getColor(icon[coord1(m, n)]));
        }
      iter = 0;
    }
  }

  void setPoint(int x, int y) {
    if ((x >= 0) && (y >= 0) && (x < w) && (y < h)) {
      short &i = icon[coord1(x, y)];
      setPoint(x, y, getColor(i));
      i++;
      if (i > 12288)
        i = 8192;
    }
  }

  void setPoint(int x, int y, int color) { screen[coord1(x, y)] = color; }

  void setNextPreset() { setPreset((npr++) % nPreset); }

  void setPreset(int i) { // lambda, alpha, beta, gamma, omega, symmetry, scale
    i = i % nPreset;
    setParameters(preset[i * 7 + 0], preset[i * 7 + 1], preset[i * 7 + 2],
                  preset[i * 7 + 3], preset[i * 7 + 4], (int)preset[i * 7 + 5],
                  preset[i * 7 + 6]);
  }

  int getnPreset() { return nPreset; }
  void getPreset(int i, double &lambda, double &alpha, double &beta,
                 double &gamma, double &omega, int &sym, double &scale) {
    i = i % nPreset;
    setParameters(preset[i * 7 + 0], preset[i * 7 + 1], preset[i * 7 + 2],
                  preset[i * 7 + 3], preset[i * 7 + 4], (int)preset[i * 7 + 5],
                  preset[i * 7 + 6]);

    lambda = this->lambda;
    alpha = this->alpha;
    beta = this->beta;
    gamma = this->gamma;
    omega = this->omega;
    sym = this->sym;
    scale = this->scale;
  }

  void setParameters(double lambda, double alpha, double beta, double gamma,
                     double omega, int sym, double scale) {

    this->lambda = lambda;
    this->alpha = alpha;
    this->beta = beta;
    this->gamma = gamma;
    this->omega = omega;
    this->sym = sym;
    this->scale = scale == 0 ? 1 : scale;
    reset();
  }

  void setParameters(
      double *pars) { // lambda, alpha, beta, gamma, omega, symmetry, scale
    setParameters(pars[0], pars[1], pars[2], pars[3], pars[4], (int)pars[5],
                  pars[6]);
  }

  double getParam(int preset, int i) { return this->preset[preset * 7 + i]; }

  bool generate(int modDisp) { // geenrate icon, runs in a thread in 'start'
    iter++;

    if (fabs(x) > MaxXY || fabs(y) > MaxXY)
      reset(); // prevent overflow

    // generate new x,y
    double sq = x * x + y * y; // sq=x^2+y^2

    double tx = x, ty = y; // tx=pow, ty=pow
    for (int m = 1; m <= sym - 2; m++) {
      double d8 = tx * x - ty * y;
      double d9 = ty * x + tx * y;
      tx = d8;
      ty = d9;
    }
    double d10 = x * tx - y * ty;
    double d11 = lambda + alpha * sq + beta * d10;
    double xNew = d11 * x + gamma * tx - omega * y;
    double yNew = d11 * y - gamma * ty + omega * x;

    x = xNew;
    y = yNew;

    if (k > 50) {
      setPoint((int)(apcx + x * rad / scale), (int)(apcy + y * rad / scale));
    } else {
      k++;
    }
    return (iter % modDisp) == 0;
  }
};
