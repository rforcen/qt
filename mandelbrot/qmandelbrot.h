#pragma once

#include <QFileDialog>
#include <QKeyEvent>
#include <QListWidget>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QPointF>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTableWidget>
#include <QWidget>

#include "cl/mandelbrotCL.h"
#include "common.h"
#include "mandel.h"

using f128 = long double;

class QMandelbrot : public QWidget {
  Q_OBJECT

public:
  explicit QMandelbrot(QWidget *parent = nullptr);
  ~QMandelbrot();

  void setStatusBar(QStatusBar *sb) { this->sb = sb; }
  void setBookMark(QTableWidget *tb) { this->tb = tb; }
  void setRootWidget(QMainWindow *wnd) { this->wnd = wnd; }
  void setExpResol(QSpinBox *expResol) { this->expResol = expResol; }

  void writeBookMark(QString fn = "mandelbrot.bmk") {
    if (tb && tb->rowCount() > 0) {
      QFile f(fn);
      f.open(QIODevice::WriteOnly);
      for (auto i = 0; i < tb->rowCount(); i++) {
        MandelDef *md = tb->item(i, 0)->data(Qt::UserRole).value<MandelDef *>();
        f.write((const char *)md, sizeof(MandelDef));
      }
      f.close();
    }
  }
  void readBookMark(QString fn = "mandelbrot.bmk") {
    if (tb) {
      QFile f(fn);
      if (f.open(QIODevice::ReadOnly)) {
        MandelDef md;

        while (f.read((char *)&md, sizeof(MandelDef)) == sizeof(MandelDef)) {
          md.get(center, range, iters);
          addbk();
        }

        f.close();
      }
    }
  }

  f128 calcOff(int off) {
    f128 ratio = (range.imag() - range.real()) / 10;
    return ratio * off / 100;
  }

private:
  Mandelbrot32 mandel32;
  Mandelbrot64 mandel64;
  Mandelbrot128 mandel128;
  Mandelbrot192 mandel192;
  Mandelbrot256 mandel256;
  Mandelbrot320 mandel320;
  MandelbrotMPFR mandel_mpfr;

  MandelbrotCL<cl_float, cl_float2> mandelCL32;

  MandelbrotCL<cl_double, cl_double2> mandelCL64;

  Complex128 center = Complex128(0.5, 0), range = Complex128(-2, 2);

  QPoint pStart, pEnd;
  bool dragging = false;
  vector<u32> image;
  MandelEngine engine = Eng_f64;

  int w, h;
  int wimg, himg; // image
  int iters;

  qreal dpr;

  QStatusBar *sb = nullptr;
  QTableWidget *tb = nullptr;
  QMainWindow *wnd = nullptr;
  QSpinBox *expResol = nullptr;

  QString fileName;

public:
  void recalc();
  void refresh() {
    recalc();
    update();
  }

  void incIters(int inc) {
    iters = std::max(iters + inc, 200);
    refresh();
  }

  template <typename T> // complex # converter
  complex<T> cconv(const Complex128 &o) {
    return complex<T>(o.real(), o.imag());
  }

  double calcScale() { return 8 / norm(range); } // 8 = norm(-2,2)

  // mandel typed
  void genMandel(int w, int h) {

    switch (engine) {
    case Eng_f32:
      image = mandel32.generate(w, h, iters, center, range);
      break;
    case Eng_f64:
      image = mandel64.generate(w, h, iters, center, range);
      break;
    case Eng_f128:
      image = mandel128.generate(w, h, iters, center, range);
      break;
    case Eng_f192_TT:
      image = mandel192.generateTT(w, h, iters, center, range);
      break;
    case Eng_f256_TT:
      image = mandel256.generateTT(w, h, iters, center, range);
      break;
    case Eng_f320_TT:
      image = mandel320.generateTT(w, h, iters, center, range);
      break;

    case Eng_f32_CL:
      image = mandelCL32.generate(w, h, cconv<cl_float>(center),
                                  cconv<cl_float>(range), iters);
      break;
    case Eng_f64_CL:
      image = mandelCL64.generate(w, h, cconv<cl_double>(center),
                                  cconv<cl_double>(range), iters);
      break;

    case Eng_f192:
      mandel_mpfr.setPrecisionBits(192);
      image = mandel_mpfr.generate(w, h, iters, center, range);
      break;
    case Eng_f256:
      mandel_mpfr.setPrecisionBits(256);
      image = mandel_mpfr.generate(w, h, iters, center, range);
      break;
    case Eng_f512:
      mandel_mpfr.setPrecisionBits(512);
      image = mandel_mpfr.generate(w, h, iters, center, range);
      break;
    case Eng_f1024:
      mandel_mpfr.setPrecisionBits(1024);
      image = mandel_mpfr.generate(w, h, iters, center, range);
      break;
    case Eng_f2048:
      mandel_mpfr.setPrecisionBits(2048);
      image = mandel_mpfr.generate(w, h, iters, center, range);
      break;
    case Eng_f4096:
      mandel_mpfr.setPrecisionBits(4096);
      image = mandel_mpfr.generate(w, h, iters, center, range);
      break;
    default:
      image.clear();
      break;
    }
  }

  QVariant getMandel() {
    return QVariant::fromValue(new MandelDef(center, range, iters));
  }

  void delMandel(QVariant data) {
    auto name = QString(data.typeName());

    if (name == "MandelDef*")
      delete data.value<MandelDef *>();
  }

  void restoreMandel(QVariant data) {
    auto name = QString(data.typeName());

    if (name == "MandelDef*")
      data.value<MandelDef *>()->get(center, range, iters);
  }

protected:
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *);

  void wheelEvent(QWheelEvent *event);

public slots:

  void bookMarkSel(int row, int col);
  void bookMarkSel(int row, int col, int pr, int pc);

  void addbk();
  void delbk();
  void clearbk();

  void setIters(int iters);
  void setEngine(int engine);
  void resetView();

  void SaveBk();
  void LoadBk();

  void Export();

  void horzScr(int off); // h/v scroll
  void vertScr(int off);
signals:
};

Q_DECLARE_METATYPE(MandelDef *);
