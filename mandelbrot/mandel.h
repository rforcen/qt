// mandel
// g++ -O3 mandel.cpp -o mandel -pthread

#pragma once

#include "Thread.h"
#include "Timer.h"
#include "spline.h"

#include <complex>
#include <memory>
#include <string>
#include <vector>

#pragma GCC diagnostic ignored "-Wuninitialized"
#include "ttmath/ttmath.h"

#include <mpreal.h>

#include "firePalette.h"

using mpfr::mpreal;
using std::complex, std::vector, std::string, std::to_string;

typedef uint32_t u32;
typedef complex<double> Complex64;
typedef complex<long double> Complex128;

typedef ttmath::Big<1, 3> float192;
typedef ttmath::Big<1, 4> float256;
typedef ttmath::Big<1, 5> float320;

using f32 = float;
using f64 = double;
using f128 = long double;

enum MandelEngine {
  Eng_f32, // c++ scalar
  Eng_f64,
  Eng_f128,

  Eng_f192_TT, // ttmath
  Eng_f256_TT,
  Eng_f320_TT,

  Eng_f32_CL, // CL
  Eng_f64_CL,

  Eng_f192, // mpfr
  Eng_f256,
  Eng_f512,
  Eng_f1024,
  Eng_f2048,
  Eng_f4096
};

// Mandelbrot
//

template <class real>

class Mandelbrot {
  typedef complex<real> ComplexReal;

public:
  int w = 0, h = 0, iters = 200;
  vector<u32> image;
  ComplexReal center = ComplexReal(0.5, 0.0), range = ComplexReal(-2.0, 2.0),
              cr;
  real rir, scale;
  FirePalette firePalette;
  u32 *palette;

private:
  inline ComplexReal do_scale(real iw, real jh) {
    return cr + rir * ComplexReal(iw, jh);
  }

public:
  explicit Mandelbrot() {}
  Mandelbrot(ComplexReal center, ComplexReal range, int iters) {
    this->center = center;
    this->range = range;
    this->iters = iters;
  }

  Mandelbrot(int w, int h, int iters, Complex64 center, Complex64 range)
      : w(w), h(h), iters(iters), center(center), range(range),
        cr(ComplexReal(range.real(), range.real())),
        rir((range.imag() - range.real())), scale(0.8 * w / h) {

    image.resize(w * h);
    firePalette.resize(iters);
    palette = firePalette.getPalette();
  }

  // mpfr
  void setPrecisionBits(int bits) {
    mpreal::set_default_prec(bits); // mpfr::digits2bits(digits));
  }
  void setDigits(int d) { mpreal::set_default_prec(mpfr::digits2bits(d)); }

  vector<u32> generate(int w, int h, int iters, Complex128 center,
                       Complex128 range) {
    this->w = w;
    this->h = h;
    this->iters = iters;
    this->center = center;
    this->range = range;
    this->cr = ComplexReal(range.real(), range.real());
    this->rir = (range.imag() - range.real());
    this->scale = 0.8 * w / h;

    image.resize(w * h);
    firePalette.resize(iters);
    palette = firePalette.getPalette();

    return maneldebrot_mt();
  }

  complex<real> toc128(Complex128 z) {
    return complex<real>(real(std::to_string(z.real())),
                         real(to_string(z.imag())));
  }

  complex<real> &operator=(Complex128 &z) {
    return complex<real>(real(to_string(z.real())), real(to_string(z.imag())));
  }
  real &operator=(long double &f) { return to_string(f); }

  vector<u32> generateTT(int w, int h, int iters, Complex128 center,
                         Complex128 range) {
    this->w = w;
    this->h = h;
    this->iters = iters;
    this->center = toc128(center);
    this->range = toc128(range);
    this->cr = ComplexReal(to_string(range.real()), to_string(range.real()));
    this->rir = to_string(range.imag() - range.real());
    this->scale = 0.8 * w / h;

    image.resize(w * h);
    firePalette.resize(iters);
    palette = firePalette.getPalette();

    return maneldebrot_mt();
  }

  int size_bytes() { return w * h * sizeof(u32); }
  int image_size() { return w * h; }

  inline u32 gen_pixel(int index) {
    const ComplexReal c0 =
        scale * do_scale(real(index % w) / w, real(index / w) / h) - center;
    ComplexReal z = c0;

    int ix = iters;
    for (int it = 0; it < iters; it++) {
      z = z * z + c0;
      if (norm(z) > real(4)) {
        ix = it;
        break;
      }
    }
    return convertARGB(0xff000000 |
                       ((ix == iters) ? 0 : palette[(ix << 2) % iters]));
  }

  inline u32 convertARGB(u32 i) {
    u32 it = i;
    uint8_t *wi4 = (uint8_t *)&i, *wti4wt = (uint8_t *)&it;
    wti4wt[0] = wi4[2];
    wti4wt[1] = wi4[1];
    wti4wt[2] = wi4[0];
    wti4wt[3] = wi4[3];
    return it;
  }

  void convertARGB() { // convert to Format_ARGB32
    Thread(image_size()).run([this](int index) {
      image[index] = convertARGB(image[index]);
    });
  }
  // single thread
  void maneldebrot_st() {
    for (auto index = 0; index < image_size(); index++)
      image[index] = gen_pixel(index);
  }

  // multithread version
  vector<u32> maneldebrot_mt() {
    Thread(image_size()).run([this](int index) {
      image[index] = gen_pixel(index);
    });

    return image;
  }

  void restore(Complex128 &center, Complex128 &range, int &iters) {
    center = this->center;
    range = this->range;
    iters = this->iters;
  }
};

using Mandelbrot32 = Mandelbrot<float>;
using Mandelbrot64 = Mandelbrot<double>;
using Mandelbrot128 = Mandelbrot<long double>;
// ttmath
using Mandelbrot192 = Mandelbrot<float192>;
using Mandelbrot256 = Mandelbrot<float256>;
using Mandelbrot320 = Mandelbrot<float320>;

// mpfr
using MandelbrotMPFR = Mandelbrot<mpreal>;

// MandelDef
class MandelDef {
public:
  MandelDef() {}

  MandelDef(Complex128 center, Complex128 range, int iters)
      : center(center), range(range), iters(iters) {}

  MandelDef(Mandelbrot32 &m)
      : center(m.center), range(m.range), iters(m.iters) {}
  MandelDef(Mandelbrot64 &m)
      : center(m.center), range(m.range), iters(m.iters) {}
  MandelDef(Mandelbrot128 &m)
      : center(m.center), range(m.range), iters(m.iters) {}

  // ttmath
  MandelDef(Mandelbrot192 &m)
      : center(toc128(m.center)), range(toc128(m.range)), iters(m.iters) {}
  MandelDef(Mandelbrot256 &m)
      : center(toc128(m.center)), range(toc128(m.range)), iters(m.iters) {}
  MandelDef(Mandelbrot320 &m)
      : center(toc128(m.center)), range(toc128(m.range)), iters(m.iters) {}

  // ttmath conversion
  template <class T> Complex128 operator=(complex<T> &z) { return toc128(z); }

  template <class T> Complex128 toc128(complex<T> z) {
    return Complex128(stold(z.real().ToString()), stold(z.imag().ToString()));
  }

  void get(Complex128 &center, Complex128 &range, int &iters) {
    center = this->center;
    range = this->range;
    iters = this->iters;
  }
  Complex128 center, range;
  int iters;
};
