#ifndef MOUSEPAINTER_H
#define MOUSEPAINTER_H

#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>

class MousePainter : public QWidget {
  Q_OBJECT
  static const int ballPenSize = 20;
  static const int smallImagSize = 28;

 public:  //
  QList<QPoint> pnts;
  QImage img, imgSmall;
  int w, h;

 public:
  explicit MousePainter(QWidget *parent = nullptr) : QWidget(parent) {}

  void mouseMoveEvent(QMouseEvent *event) {
    switch (event->buttons()) {
      case Qt::LeftButton:
        pnts << event->pos();
        repaint();
        break;
      case Qt::RightButton:
        pnts.clear();
        repaint();
        break;
    }
  }
  void mousePressEvent(QMouseEvent *) { pnts.clear(); }
  void mouseReleaseEvent(QMouseEvent *) {
    img.fill(Qt::black);

    QPainter p(&img);
    draw(p);

    repaint();

    emit digitFinished(imgSmall);
  }
  void paintEvent(QPaintEvent *) {
    QPainter p(this);

    w = p.device()->width();
    h = p.device()->height();

    if (!img.isNull())
      p.drawImage(0, 0, imgSmall = img.scaled(smallImagSize, smallImagSize));
    else {
      img = QImage(w, h, QImage::Format_Mono);
      img.fill(Qt::black);
    }
    draw(p);
  }
  void draw(QPainter &p) {
    p.setBrush(QBrush(Qt::black));
    p.setPen(Qt::NoPen);

    for (auto pnt : pnts) p.drawEllipse(pnt, ballPenSize, ballPenSize);
  }

 signals:
  void digitFinished(QImage img);
 public slots:
};

#endif  // MOUSEPAINTER_H
