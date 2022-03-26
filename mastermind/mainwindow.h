#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mastermind.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  MasterMind mm;

  QString toString(vector<int> v) {
    QString s;
    for (auto i : v)
      s += QString::number(i);
    return s;
  }

  void showPerms();

public slots:
  void selectPerm(int);
  void clear();
  void check();
};
#endif // MAINWINDOW_H
