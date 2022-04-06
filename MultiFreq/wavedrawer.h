#ifndef WAVEDRAWER_H
#define WAVEDRAWER_H

#include <QMouseEvent>
#include <QPainter>
#include <QWidget>

#include <d.h>

class WaveDrawer : public QWidget {
  Q_OBJECT

 public:
  explicit WaveDrawer(QWidget *parent = nullptr) : QWidget(parent) {
    connect(&d, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
  }

  void setType(D::WaveComponent who) { this->who = who; }

 private:
  QPoint pnt;
  bool newPoint = false;
  int w, h, x, y, np;
  int who = -1;

 private:
  void drawLineTo(const QPoint &pnt) {
    this->pnt = pnt;
    newPoint = true;
    update();

    emit dataUpdated();  // wave data has changed
  }

 signals:
  void dataUpdated();

 public slots:
  void dataChanged() { update(); }

 protected:
  void mousePressEvent(QMouseEvent *) {}
  void mouseReleaseEvent(QMouseEvent *) {}

  void mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton)) drawLineTo(event->pos());
  }
  void paintEvent(QPaintEvent *) {
    QPainter painter(this);

    if (who == -1)  // not init yet
      who =
          d.findType(this->accessibleName());  // find type using accesible name

    w = painter.device()->width(), h = painter.device()->height();

    painter.drawRect(0, 0, w, h);  // main frame

    y = pnt.y(), x = pnt.x();

    x = (x < 0) ? 0 : (x >= w) ? w : x;
    y = (y < 0) ? 0 : (y >= h) ? h - 1 : y;

    np = (d.nWaves * y) / h;
    if (newPoint)
      d.setData(who, np, 1. * x / w);  // d.xProp[who][np] = 1. * x / w;

    paintBars(painter);

    newPoint = false;
  }

  void paintBars(QPainter &painter) {
    for (int row = 0; row < d.nWaves; row++) {
      if (d.xProp[who][row]) {
        auto rect =
            QRect(0, (row * h) / d.nWaves, d.xProp[who][row] * w, h / d.nWaves);
        painter.setPen(Qt::white);  // frame
        painter.drawRect(rect);

        painter.fillRect(rect, d.brush[who]);  // bar fill

        painter.setPen((d.brightness(d.brush[who]) > 0.6) ? Qt::black
                                                        : Qt::white);  // text
        QFont font("arial", 7);
        painter.setFont(font);
        auto s = QString("%1").arg(d.getData(who, row), 0, 'F', 0);
        rect.setWidth(QFontMetrics(font).width(s));
        painter.drawText(rect, s);
      }
    }
  }

  void paintPoly(QPainter &painter) {
    QPolygon poly;

    poly << QPoint(0, 0);  // starts at 0,0
    for (int i = 0; i < d.nWaves; i++)
      poly << QPoint(d.xProp[who][i] * w, (i * h) / d.nWaves);
    poly << QPoint(0, h);  // ends at 0,h

    painter.setBrush(d.brush[who]);
    painter.drawPolygon(poly);
  }
};

#endif  // WAVEDRAWER_H
