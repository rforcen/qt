#ifndef QIMAGESHOW_H
#define QIMAGESHOW_H

#include <QPainter>
#include <QWidget>

class QImageShow : public QWidget {
  Q_OBJECT
public:
  explicit QImageShow(QWidget *parent = nullptr);

  void setImage(const QImage img) {
    this->img = img;
    update();
  }

private:
  QImage img;
  qreal dpr;

protected:
  void paintEvent(QPaintEvent *) override;
  void mousePressEvent(QMouseEvent *event) override;

signals:
  void clicked(QMouseEvent *);
};

#endif // QIMAGESHOW_H
