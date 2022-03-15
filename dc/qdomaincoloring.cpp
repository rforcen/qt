#include "qdomaincoloring.h"
#include "common.h"
#include "dc/Timer.h"

QDomainColoring::QDomainColoring(QWidget *parent) : QWidget{parent} {
  dpr = devicePixelRatioF();
}

void QDomainColoring::setPreset(int ps) {
  expr = DomainColoring::getPreset(ps);
  update();
}

void QDomainColoring::setExpression(QString exp) {
  expr = exp.toStdString();
  if (dc.compile(expr)) {
    recalc();
    update();
  }
}

void QDomainColoring::Export(int resol) {
  image = dc.generate(resol, resol, expr);
  QImage img((uchar *)image.data(), resol, resol, QImage::Format_ARGB32);

  if (img.save("dc.png"))
    emit sendMessage("file dc.png exported");
}

void QDomainColoring::recalc() {
  setSize();

  Timer t0;
  image = dc.generate(wimg, himg, expr);

  emit sendMessage(sformat("[%s] lap: %ldms, size:(%d, %d)",
                           dc.getErrorMsg().c_str(), t0.lap(), wimg, himg));
}

void QDomainColoring::setSize() {
  w = width();
  h = height();

  wimg = w * dpr;
  himg = h * dpr;
}

void QDomainColoring::resizeEvent(QResizeEvent *) { recalc(); }

void QDomainColoring::paintEvent(QPaintEvent *) {

  QPainter p(this);
  p.scale(1 / dpr, 1 / dpr);

  QImage img((uchar *)image.data(), wimg, himg, QImage::Format_ARGB32);

  p.drawImage(0, 0, img);
}
