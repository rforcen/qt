#ifndef WAVEGRAPH_H
#define WAVEGRAPH_H

#include <d.h>
#include <math.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <climits>

class WaveGraph : public QWidget {
  Q_OBJECT
 public:
  explicit WaveGraph(QWidget *parent = nullptr);
  void paintEvent(QPaintEvent *);
 signals:

 public slots:
};

#endif  // WAVEGRAPH_H
