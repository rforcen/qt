#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <mnist.h>
#include <nn.h>

#include <QMainWindow>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void onEpoch(int epoch, double hitFactor);
  void onDigitDraw(QImage img);

  void on_itemTrain_valueChanged(int value);
  void on_itemTest_valueChanged(int value);
  void on_actiontest_triggered();
  void on_actionlearn_triggered();
  void on_actionsave_training_triggered();
  void on_actionread_training_triggered();

private:
  Ui::MainWindow *ui;

  NN *nn;
  int nEpochs=20;
  QTime time;

  MNIST mnist;
  std::thread tLearn;
private:
  void doLearn();
};

#endif  // MAINWINDOW_H
