#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QVector>
#include <wordle.h>

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

  Wordle wordle;

  void loadDict();

public slots:
  void changeLetters(int);
  void check();
  void dictSel(const QModelIndex &index);
  void clear();
};
#endif // MAINWINDOW_H
