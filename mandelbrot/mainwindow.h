#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>

#include <string>
using std::string;

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

  QSettings *settings = nullptr;
  void loadSettings(), saveSettings();

protected:
  void keyPressEvent(QKeyEvent *event);

public slots:
  void horzScr(int action); // h/v scroll
  void vertScr(int action);
};
#endif // MAINWINDOW_H
