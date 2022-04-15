#include "qimageshow.h"

QImageShow::QImageShow(QWidget *parent) : QWidget{parent} {
  dpr = devicePixelRatioF();
}

void QImageShow::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.scale(1 / dpr, 1 / dpr);

  if (!img.isNull()) {
    p.drawImage(0, 0, img.scaled(size()));
  }
}

void QImageShow::mousePressEvent(QMouseEvent *event) { emit clicked(event); }
