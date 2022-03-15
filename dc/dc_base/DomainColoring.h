#pragma once

#include "Thread.h"
#include "zCompiler.h"
#include <complex>
#include <string>
#include <vector>

using Complex64 = std::complex<double>;
using std::string;
using std::vector;
using u32 = uint32_t;

static vector<string> presets = {"acos((1+i)*log(sin(z^3-1)/z))",
                                 "(1+i)*log(sin(z^3-1)/z)",
                                 "(1+i)*sin(z)",
                                 "z + z^2/sin(z^4-1)",
                                 "log(sin(z))",
                                 "cos(z)/(sin(z^4-1)",
                                 "z^6-1",
                                 "(z^2-1) * (z-2-i)^2 / (z^2+2*i)",
                                 "sin(z)*c(1,2)",
                                 "sin(1/z)",
                                 "sin(z)*sin(1/z)",
                                 "1/sin(1/sin(z))",
                                 "z",
                                 "(z^2+1)/(z^2-1)",
                                 "(z^2+1)/z",
                                 "(z+3)*(z+1)^2",
                                 "(z/2)^2*(z+1-2i)*(z+2+2i)/z^3",
                                 "(z^2)-0.75-(0.2*i)",
                                 "z * sin( c(1,1)/cos(3/z) + tan(1/z+1) )"};

class DomainColoring {
private:
  Complex64 ci = Complex64(0, 1);
  zCompiler zComp;

  vector<u32> image;
  int w, h;

public:
  DomainColoring(){};
  DomainColoring(int w, int h, string expr) : w(w), h(h) {
    zComp.compile(expr);
  }
  DomainColoring(int w, int h, int ipreset) : w(w), h(h) {
    zComp.compile(presets[ipreset % (int)presets.size()]);
  }

  vector<uint32_t> generate(int w, int h, string expr) {

    this->w = w;
    this->h = h;
    image.resize(w * h);

    zComp.compile(expr);
    return genImageMT();
  }

  bool compile(string expr) {
    zComp.compile(expr);
    return !getError();
  }
  string getErrorMsg() { return getError() ? zComp.errMessage : "ok"; }

  bool getError() { return zComp.err; }

  static string getPreset(int i) { return presets[i % (int)presets.size()]; }

  inline double pow3(double x) { return x * x * x; }

  vector<u32> genImage() {
    const double PI2 = M_PI * 2;
    double limit = M_PI, rmi, rma, imi, ima;

    rmi = -limit, rma = limit, imi = -limit, ima = limit;

    try {
      for (int j = 0; j < h; j++) {
        double im = ima - (ima - imi) * j / (h - 1);
        for (int i = 0; i < w; ++i) {
          double re = rma - (rma - rmi) * i / (w - 1);

          Complex64 v = zComp.execute(Complex64(re, im)); // evaluate here

          double a = arg(v);
          while (a < 0)
            a += PI2;
          a /= PI2;
          double m = abs(v), ranges = 0, rangee = 1;
          while (m > rangee) {
            ranges = rangee;
            rangee *= M_E;
          }

          double k = (m - ranges) / (rangee - ranges);
          double kk = (k < 0.5 ? k * 2 : 1 - (k - 0.5) * 2);

          double sat = 0.4 + (1 - pow3((1 - (kk)))) * 0.6;
          double val = 0.6 + (1 - pow3((1 - (1 - kk)))) * 0.4;

          image[j * w + i] = HSV2int(a, sat, val);
        }
      }
    } catch (...) {
    }
    return image;
  }

  vector<u32> genImageMT() {
    const double PI2 = M_PI * 2;
    double limit = M_PI, rmi, rma, imi, ima;

    rmi = -limit, rma = limit, imi = -limit, ima = limit;

    try {
      Thread(w * h).run([this, rmi, rma, imi, ima, PI2](int th, int index) {
        (void)th;

        int j = index / w, i = index % w;

        double im = ima - (ima - imi) * j / (h - 1),
               re = rma - (rma - rmi) * i / (w - 1);

        Complex64 v = zComp.execute(Complex64(re, im)); // evaluate here

        double a = arg(v);
        while (a < 0)
          a += PI2;
        a /= PI2;
        double m = abs(v), ranges = 0, rangee = 1;
        while (m > rangee) {
          ranges = rangee;
          rangee *= M_E;
        }

        double k = (m - ranges) / (rangee - ranges);
        double kk = (k < 0.5 ? k * 2 : 1 - (k - 0.5) * 2);

        double sat = 0.4 + (1 - pow3((1 - (kk)))) * 0.6;
        double val = 0.6 + (1 - pow3((1 - (1 - kk)))) * 0.4;

        image[index] = HSV2int(a, sat, val);
      });
    } catch (...) {
    }

    return image;
  }

  int HSV2int(double h, double s, double v) {
    // convert hsv to int with alpha 0xff00000
    double r = 0, g = 0, b = 0;
    if (s == 0)
      r = g = b = v;
    else {
      if (h == 1)
        h = 0;
      double z = floor(h * 6);
      int i = (int)(z);
      double f = h * 6 - z, p = v * (1 - s), q = v * (1 - s * f),
             t = v * (1 - s * (1 - f));

      switch (i) {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      case 5:
        r = v;
        g = p;
        b = q;
        break;
      }
    }
    int c, color = 0xff000000;
    // alpha = 0xff
    c = (int)(255 * r) & 0xff;
    color |= c;
    c = (int)(255 * g) & 0xff;
    color |= (c << 8);
    c = (int)(255 * b) & 0xff;
    color |= (c << 16);
    return color;
  }
};
