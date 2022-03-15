#pragma once

#include "dc/DomainColoring.h"
#include <QPainter>
#include <QWidget>

class QDomainColoring : public QWidget {
  Q_OBJECT
public:
  explicit QDomainColoring(QWidget *parent = nullptr);

  void setPreset(int ps);
  void setExpression(QString exp);
  void Export(int resol);

private:
  DomainColoring dc;

  int w, h;
  int wimg, himg;
  vector<u32> image;
  string expr = DomainColoring::getPreset(0);

  qreal dpr = 1;

private:
  void recalc();
  void setSize();

protected:
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *);
signals:
  void sendMessage(QString);
};
