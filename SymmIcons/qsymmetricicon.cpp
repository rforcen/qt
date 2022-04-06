#include "qsymmetricicon.h"

QSymmetricIcon::QSymmetricIcon(QWidget *parent) : QWidget{parent} {
  dpr = devicePixelRatioF();

  connect(this, &QSymmetricIcon::Repaint, this, [this]() { update(); });
}

// events

void QSymmetricIcon::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.scale(1 / dpr, 1 / dpr);

  p.drawImage(
      0, 0,
      QImage((uchar *)si.screen.data(), wimg, himg, QImage::Format_ARGB32));
}

void QSymmetricIcon::resizeEvent(QResizeEvent *) {
  wimg = dpr * width();
  himg = dpr * height();

  stop();

  si.setSize(wimg, himg);

  run();
}
