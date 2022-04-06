#ifndef QSYMMETRICICON_H
#define QSYMMETRICICON_H

#include <QAtomicInt>
#include <QModelIndex>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QWidget>

#include <SymmIcon.h>
#include <thread>

class QSymmetricIcon : public QWidget {
  Q_OBJECT
public:
  explicit QSymmetricIcon(QWidget *parent = nullptr);
  ~QSymmetricIcon() { stop(); }

  SymmetricIcon si;

  void setParams(double *params) {
    stop();
    si.setParameters(params);
    run();
  }

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *) override;

private:
  qreal dpr;
  int wimg, himg;
  QAtomicInt running = false, ack_stop;

  void stop() {
    if (running) {
      ack_stop = false;
      running = false;
      while (!ack_stop)
        ;
    }
  }

  void run() {
    stop();

    std::thread([this]() {
      for (running = true; running;)
        if (si.generate(10000)) {
          emit Repaint();
          if (si.iter % 10000000 == 0)
            emit Progress(
                QString("generated %1 Msamples").arg(si.iter / 1000000));
        }
      ack_stop = true;
    }).detach();
  }

  void mousePressEvent(QMouseEvent *event) override {
    if (running)
      stop();
    else
      run();
  }

signals:
  void Repaint();
  void Progress(QString);
};

#endif // QSYMMETRICICON_H
